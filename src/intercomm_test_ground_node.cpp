#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 
// #include <sys/stat.h>
#include "iperf_wrapper.h"
#include "Drone_Command.h"

int main(int argc,char* argv[]){

	// Intercommunication icm("10.64.44.70","10.64.44.71");
    sleep(2);
    
    bool is_server;
	std::string iperf_file;
	std::string server_ip;
	std::string client_ip;
	std::string bw;
	
	if(argc<3){
		std::cout<<RED<<"ARGUMENTS: -iperf_file -is_server -server_ip -client_ip -bw\n"<<RESET;
		return -1;
	}else{

		iperf_file = std::string(argv[1]);
		is_server= std::string(argv[2])=="1"?true:false;
		server_ip=std::string(argv[3]);
		client_ip = std::string(argv[4]);
		bw =std::string(argv[5]);
	}
	
	Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;
	int drone_landed_state = 0;

    iperf_wrapper wrpr(iperf_file,is_server,server_ip,client_ip,bw);

	dc_A.initialise_intercomm("192.168.200.1","192.168.200.10",14551,14550,false);
	
    dc_A.enable_msg_interval(MAVLINK_MSG_ID_HEARTBEAT);
	dc_A.enable_msg_interval(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
    

	printf("Waiting for heartbit\n");
	dc_A.get_heartbit();
	
	std::thread receiver_thread([&](){
		while(!dc_A.termination_is_requested()){
		
			//receive position from FMU, if gps is received it will be sent to the other node
			dc_A.recv_data();// from drone
		}
	});

	


	// wrpr.iperf_start();
	std::thread termination_thread([&](){
		char c;

		c=getchar();

		if(c=='q'){
		
			dc_A.request_termination();

			mavlink_global_position_int_t fake;
			dc_A.get_icm_obj()->send_gps_position(dc_A.get_system_id(),dc_A.get_component_id(),fake,true);
			// std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
			return 0;
		}



	});	
	
	while(!dc_A.termination_is_requested()){
		// mavlink_global_position_int_t meas;
		mavlink_peer_position_t meas;
		//receive peer_gps_position
		if(dc_A.get_icm_obj()->receive_gps_position(&meas)){
			dc_A.send_message(dc_A.get_system_id(),dc_A.get_component_id(),meas);
		}

			mavlink_global_position_int_t global_pos;

			dc_A.get_position(&global_pos);
			printf("My lat %f my lon %f my alt %f my relative alt %d\n",(double)global_pos.lat*1.e-7,(double)global_pos.lon*1.e-7,(float)global_pos.alt*1.e-3f,global_pos.relative_alt);
			printf("peer lat %f peer lon %f peer alt %f peer relative alt %d\n",(double)meas.lat*1.e-7,(double)meas.lon*1.e-7,(float)meas.alt*1.e-3f,meas.relative_alt);

			// global_pos.alt -=1000;
			// global_pos.relative_alt -=20;
			// global_pos.lon=global_pos.lon-0.9;
			global_pos.relative_alt= 2.0*1000.0f;
			// global_pos.lat = meas.lat;
			dc_A.get_icm_obj()->send_gps_position(dc_A.get_system_id(),dc_A.get_component_id(),global_pos,false);
			printf("position sent\n");
		
	}

	receiver_thread.join();
	termination_thread.join();

    return 0;
}