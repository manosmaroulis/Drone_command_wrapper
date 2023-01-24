#include "Drone_Command.h"

using namespace std;

Drone_Command::Drone_Command():serial_port((char*)UARTNAME, BAUDRATE),companion_id(0),system_id(-1),component_id(-1),landed_state(0)
{

	icm_initialised= false;
	//initialisation of the serial communication:
	Debug(GREEN,"Serial - interface start\n",RESET);
	try
	{

	serial_port.start();	
	
	}
	catch(const int e)
	{
		cerr<<RED<<"MAKE SURE THE SERIAL PORT IS CONNECTED, TRYING NEXT PORT \n"<<RESET;
		// std::cerr << e.what() << '\n';
		// exit(EXIT_FAILURE);
		serial_port.uart_name = (char*)UARTNAME_2;
		try{

			serial_port.start();
		}
		catch(const int k){
			cerr<<RED<<"MAKE SURE THE SERIAL PORT IS CONNECTED,TERMINATING \n"<<RESET;
			exit(EXIT_FAILURE);
		}
	}
	
	
	Debug(GREEN,"Drone_Command serial connection established\n",RESET);
	is_not_end = true;

	gps_file.open("gps_log.txt");
	gps_file<<"LAT "<< "LON "<<"ALT "<<"TIME SINCE BOOT \n";
}


Drone_Command::~Drone_Command(){
	gps_file<<"END\n";
	gps_file.flush();
	gps_file.close();
	serial_port.stop();
	icm.reset();
	
}

void Drone_Command::initialise_intercomm(char* myIP, char*peerIP,int myPort, int peerPort,bool rx_only){
	// PUT A GUARD HERE
	icm = std::unique_ptr<Intercommunication>(new Intercommunication(myIP,peerIP,myPort,peerPort));
	icm_initialised = true;
	icm_rx_only = rx_only;

}

bool Drone_Command::termination_is_requested(){
	std::lock_guard<std::mutex> lock(mtx);
	if(is_not_end)
		return false;
	else
		return true;
}

void Drone_Command::request_termination(){
	std::lock_guard<std::mutex> lock(mtx);
	is_not_end = false;
}


void Drone_Command::get_position(mavlink_global_position_int_t*pos){
	std::lock_guard<std::mutex> lock(mtx);
	pos->alt = global_pos.alt;
	pos->lat = global_pos.lat;
	pos->lon = global_pos.lon;
	pos->hdg = global_pos.hdg;
	pos->relative_alt = global_pos.relative_alt;
	pos->time_boot_ms = global_pos.time_boot_ms;
	pos->vx = global_pos.vx;
	pos->vy = global_pos.vy;
	pos->vz = global_pos.vz;

	// return global_pos;
}

void Drone_Command::set_position(mavlink_global_position_int_t*pos){
	std::lock_guard<std::mutex> lock(mtx);
	global_pos.alt = pos->alt;//*1.e-3f;
	global_pos.lat = pos->lat;//*1.e-7;
	global_pos.lon = pos->lon;//*1.e-7;
	global_pos.hdg = pos->hdg;
	global_pos.relative_alt = pos->relative_alt;
	global_pos.time_boot_ms=pos->time_boot_ms;
	global_pos.vx = pos->vx;
	global_pos.vy = pos->vy;
	global_pos.vz = pos->vz;
}



void Drone_Command::get_heartbit(){	


	//decouple read writing operations
	while(system_id<0){
		recv_data();
	}

	Debug(WHITE,"System id is "<<(system_id),RESET);
	Debug(WHITE,"Component id is "<<(component_id),RESET);
	Debug(WHITE,"Companion computer id is "<<(companion_id),RESET);
	Debug(GREEN," System is ready \n",RESET);

	return;
}


int Drone_Command::get_mode()
{
	std::lock_guard<std::mutex> lock(mtx);
	return(r_mode);
}

int Drone_Command::get_armed()
{
	std::lock_guard<std::mutex> lock(mtx);
	return(r_armed);
}

int Drone_Command::get_landed_state(){


	return landed_state.load(std::memory_order_relaxed);
}

void Drone_Command::arm(bool state){ 
	std::lock_guard<std::mutex> lock(mtx);
	if(state)
		r_armed = 1;
	else
		r_armed = 0;
}


int Drone_Command::send_command(uint8_t target_system,uint8_t target_component,uint16_t command,bool confirmation,float param1,float param2){
	mavlink_message_t msg;
	mavlink_command_long_t cmd = {0};
	cmd.target_system = target_system;
	cmd.target_component =target_component;
	cmd.command = command;
	cmd.confirmation = confirmation;

	if(param1 != -1){
		cmd.param1 = param1;
	}
	if(param2 != -1){
		cmd.param2 = param2;
	}

	mavlink_msg_command_long_encode(target_system,companion_id,&msg,&cmd);
	
	//decouple reader writer
	std::lock_guard<std::mutex> lock(rw_mtx);
	
	int len = serial_port.write_message(msg);
	if(len == 0){
		Debug(RED,"COULD NOT WRITE COMMAND "<<(command)+".Trying again..\n",RESET);
		len = serial_port.write_message(msg);
	}

	return len;
}

int Drone_Command::send_message(uint8_t target_system,uint8_t target_component,mavlink_peer_position_t& position_message){
	//decouple reader writer
	mavlink_message_t message;
	// mavlink_msg_global_position_int_encode(target_system,target_component,&message,&position_message);
	mavlink_msg_peer_position_encode(target_system,target_component,&message,&position_message);
	

	std::cout<<message.msgid<<" MESSAGE ID SENDING TO AUTOPILOT\n";
	std::lock_guard<std::mutex> lock(rw_mtx);
	


	int len = serial_port.write_message(message);
	if(len == 0){
		Debug(RED,"COULD NOT WRITE COMMAND.Trying again..\n",RESET);
		len = serial_port.write_message(message);
	}

	return len;

}



int Drone_Command::enable_msg_interval(int msg_id){

// MAVLINK_MSG_ID_GLOBAL_POSITION_INT
	//param 2 = rate, default_rate =0
	int len = send_command(system_id,component_id,MAV_CMD_SET_MESSAGE_INTERVAL,true,msg_id,2000000);
	
	return len;
}

int Drone_Command::send_arm_command(int state)
{
	
	int len = send_command(system_id,component_id,MAV_CMD_COMPONENT_ARM_DISARM,true,(int)state);//param2 = 21196 bypasses checks

	return len;
}

int Drone_Command::toggle_offboard_control(bool flag){
	
	int len = send_command(system_id,component_id,MAV_CMD_DO_SET_MODE,true,(int)MAV_CMD_NAV_GUIDED_ENABLE);
	
	return len;
}

int Drone_Command::mission_start(){
	
	int len = send_command(system_id,component_id,MAV_CMD_DO_SET_MODE,true,MAV_MODE_AUTO_DISARMED);

	return len;
}

int Drone_Command::hold_to_waypoint(){


	// hold.command = MAV_CMD_NAV_LOITER_UNLIM;
	// hold.confirmation = true;
	// hold.param3 =  //Radius around mission in meters
	// hold.param4 = // desired yaw angle
	// hold.param5 = // latitude
	// hold.param6 = //longitude
	// hold.param7 = attitude
	int len = send_command(system_id,component_id,MAV_CMD_NAV_LOITER_UNLIM,true);

	return len;
}

int Drone_Command::test_command(){

	// com.command = 0;// PX4_CUSTOM_COMMAND_TEST
	int len = send_command(system_id,component_id,0,true);
	return len;
}

message_result Drone_Command::handle_message(mavlink_message_t  *msg)
{
	message_result res;
	switch (msg->msgid)
	{
		case MAVLINK_MSG_ID_HEARTBEAT:
		{
			mavlink_heartbeat_t hb;

			if(system_id<0){
				Debug(GREEN,"Initializing system ID\n",RESET);
				std::lock_guard<std::mutex> lock(mtx);
				system_id = msg->sysid;
				component_id = msg->compid;
			}
			

			Debug(GREEN,"MAVLINK_MSG_ID_HEARTBEAT\n",RESET);

			mavlink_msg_heartbeat_decode(msg, &hb);
			
			// Debug(WHITE,"SystemID: "<<msg->sysid<<"\n",RESET);
			// Debug(WHITE,"Component ID:"<< msg->compid<<"\n",RESET);
			// Debug(WHITE,"system status:"<< hb.system_status<<"\n",RESET);
			// Debug(WHITE,"custom mode:"<< hb.custom_mode<<"\n",RESET);
			// Debug(WHITE,"autopilot:"<< hb.autopilot<<"\n",RESET);
			// Debug(WHITE,"type:"<< hb.type<<"\n",RESET);
			
			// Check the arm status:
			// int armed_state = hb.base_mode & MAV_MODE_FLAG_SAFETY_ARMED;
			
			
			// Setup the Drone_Command parameters:
			r_mode = hb.custom_mode;
			// r_armed = armed_state;
			res = message_result(0,-1,-1);
			return(res);
		}
		case MAVLINK_MSG_ID_COMMAND_ACK:{
			mavlink_command_ack_t ack;
			mavlink_msg_command_ack_decode(msg,&ack);

			if(ack.command == MAV_CMD_COMPONENT_ARM_DISARM){
	
				if(ack.result == 0){
					arm(1);
					Debug(GREEN,"Vehicle is armed\n",RESET);
				}else if(ack.result == 4){
					arm(0);
					Debug(RED,"Vehicle is NOT armed\n",RESET);
					// printf("Vehicle is NOT armed\n");
				}

			}

			if(ack.command == MAV_CMD_DO_SET_MODE){


				if(ack.result==0){
					Debug(GREEN,"MODE CHANGED\n",RESET);
					// printf("MODE CHANGED\n");
				}else if( ack.result == 4){
					Debug(RED,"MODE CHANGE command executed but failed\n",RESET);
					// printf("MODE CHANGE command executed but failed\n");
				}
			}

			Debug(WHITE,"command "<< ack.command<<"\n",RESET);
			Debug(WHITE,"progess "<< ack.progress<<"\n",RESET);
			Debug(WHITE,"result "<< ack.result<<"\n",RESET);
			Debug(WHITE,"result param2 "<< ack.result_param2<<"\n",RESET);
			Debug(WHITE,"target component "<< ack.target_component<<"\n",RESET);
			Debug(WHITE,"target system "<< ack.target_system<<"\n",RESET);

			res = message_result(1,ack.command,ack.result);
			return(res);
		}
		case MAVLINK_MSG_ID_SYS_STATUS:
		{
			Debug(WHITE,"MAVLINK SYSTEM STATUS\n",RESET);
			// mavlink_msg_sys
			mavlink_sys_status_t st;

			mavlink_msg_sys_status_decode(msg,&st);
			
			// printf(st.)
			res = message_result(0,-1,-1);
			return (res);
		}
		case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
		{
			mavlink_global_position_int_t pos;
			mavlink_msg_global_position_int_decode(msg,&pos);
			Debug(GREEN,"GPS READING\n",RESET);
			// Debug(<<"TIME SINCE BOOT: "<<global_pos.time_boot_ms<<"\n";
			// Debug(<<"ALT: "<<global_pos.alt<<"\n";
			// Debug(<<"LONG: "<<global_pos.lon<<"\n";
			// Debug(<<"LAT: "<< global_pos.lat<<"\n";
			//send data to other drone
			if(is_icm_initialised() && !rx_only()){
				Debug(GREEN,"position sent\n",RESET);
				icm->send_gps_position(get_system_id(),get_component_id(),pos,false);
			}
			if(is_icm_initialised()){
				set_position(&pos);
			}

			// gps_file <<global_pos.lat<<" "<< global_pos.lon<<" "<<global_pos.alt<<" "<<global_pos.time_boot_ms<<"\n";
			// gps_file.flush();
			res = message_result(0,-1,-1);
			return res;
		}	
		case MAVLINK_MSG_ID_EXTENDED_SYS_STATE:
		{
			mavlink_extended_sys_state_t sys_state;
			mavlink_msg_extended_sys_state_decode(msg,&sys_state);
			Debug(GREEN,"EXTENDED STATE\n",RESET);

			if(sys_state.landed_state!=landed_state){
				landed_state.store(sys_state.landed_state,std::memory_order_relaxed);	
			}

			if(sys_state.landed_state==MAV_LANDED_STATE_IN_AIR){

				Debug(GREEN,"system is on air\n",RESET);

			}else if(sys_state.landed_state==MAV_LANDED_STATE_ON_GROUND){
				
				Debug(GREEN,"system is on ground\n",RESET);

			}else if(sys_state.landed_state==MAV_LANDED_STATE_LANDING){
				
				Debug(GREEN,"system is landing\n",RESET);

			}else if(sys_state.landed_state==MAV_LANDED_STATE_TAKEOFF){

				Debug(GREEN,"system is taking off\n",RESET);

			}

		}
		default:
		{
			// printf("Warning, did not handle message id %i\n",msg->msgid);
			res = message_result(0,-1,-1);
			return(res);
		}
	}		
}

bool Drone_Command::serial_read(mavlink_message_t& msg){
	// bool success;

	//decouple read writing operations
	std::lock_guard<std::mutex> lock(rw_mtx);

	return 	serial_port.read_message(msg); 
}

message_result Drone_Command::recv_data()
{
	bool success;
	mavlink_message_t msg;

	// success = serial_port.read_message(msg); 
	success = serial_read(msg);

	if(success)
	{
		message_result id = handle_message(&msg);
		return(id);
	}
	message_result id = message_result(-1,-1,-1);
	return(id);

}


int Drone_Command::get_system_id(){
	return system_id;
}

int Drone_Command::get_component_id(){
	return component_id;
}

std::unique_ptr<Intercommunication>& Drone_Command::get_icm_obj(){
	return icm;
}
bool Drone_Command::is_icm_initialised(){
	return icm_initialised;
}
bool Drone_Command::rx_only(){
	return icm_rx_only;
}
// int Drone_Command::guided_mode()
// {
// 	// Format the data:
// 	mavlink_command_long_t set_mode = {0};
// 	set_mode.target_system = 1;
// 	set_mode.target_component = 0;
// 	set_mode.command = MAV_CMD_DO_SET_MODE;		//176
// 	set_mode.confirmation = true;
// 	set_mode.param1 = 1; 				//need to be 1 ?? check			 	
// 	set_mode.param2 = GUIDED; 			//guided_mode: 4 for drone / 15 for Drone_Command... 
	
// 	// Encode:
// 	mavlink_message_t msg;
// 	mavlink_msg_command_long_encode(1, 255, &msg, &set_mode);
		
// 	// Write in the serial:
// 	int len = serial_port.write_message(msg);
// 	//printf("Guided mode (%d)\n", len);
// 	return(len);
// }


