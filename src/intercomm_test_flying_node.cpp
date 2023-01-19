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
	
	char c;
	Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;
	int drone_landed_state = 0;

    iperf_wrapper wrpr(iperf_file,is_server,server_ip,client_ip,bw);

	dc_A.initialise_intercomm("192.168.200.10","192.168.200.1",14550,14551,true);
	
    dc_A.enable_msg_interval(MAVLINK_MSG_ID_HEARTBEAT);
	dc_A.enable_msg_interval(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
    

	printf("Waiting for heartbit\n");
	dc_A.get_heartbit();
	
	std::thread receiver_thread([&](){
		while(!dc_A.termination_is_requested()){
			mavlink_peer_position_t meas;
			//receive peer_gps_position
			if(dc_A.get_icm_obj()->receive_gps_position(meas)){
    			
				if(meas.static_pos){
					//currently used to stop the system
					break;
				}

				dc_A.send_message(dc_A.get_system_id(),dc_A.get_component_id(),meas);			//send to FMU 
				//send to FMU 
            }
			//receive position from FMU, if gps is received it will be sent to the other node
			dc_A.recv_data();// from drone
		}
	});

	wrpr.iperf_start();

	// c=getchar();
	receiver_thread.join();
	wrpr.iperf_stop();

		// dc_A.request_termination();

	// if(c=='q'){
		// dc_A.send_arm_command(0);
		
		// std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
		// return 0;
	// }
 
    return 0;
}