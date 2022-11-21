#include "Drone_Command.h"
#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 
// #include <sys/stat.h>
#include "iperf_wrapper.h"
#include "Intercommunication.h"
// #define MIN_LEN 200

// long GetFileSize(std::string filename)
// {
//     struct stat stat_buf;
//     int rc = stat(filename.c_str(), &stat_buf);
//     return rc == 0 ? stat_buf.st_size : -1;

// }

void close_app(){


}




int main(int argc, char**argv)
{

	Intercommunication icm("10.64.44.70","10.64.44.71");
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
	// Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;
	int drone_landed_state = 0;




	iperf_wrapper wrpr(iperf_file,is_server,server_ip,client_ip,bw);
	
	// dc_A.enable_msg_interval(MAVLINK_MSG_ID_HEARTBEAT);
	// dc_A.enable_msg_interval(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
	
	// MAVLINK_MSG_ID_EXTENDED_SYS_STATE
	// printf("Waiting for heartbit\n");
	// dc_A.get_heartbit();

	
	/*Start a background thread to receive messages*/

	// std::thread receiver_thread([&](){
	// 	while(!dc_A.termination_is_requested()){
	// 		dc_A.recv_data();
	// 	}
	// });


	//CHECK WIFI INTERFACES FOR A CONNECTION
	wic.find_interfaces();
	auto names = wic.get_wlan_names();

	for(auto& name: names){
		
		char* c_name = &name[0];//CHANGE THIS TO HARDCODE WIFI INTERFACE
					   //"wlan0"
		
		int result = wic.getSignalInfo(&sigInfo,c_name);

		if(result==1){
			printf("Connection established\n");
			sigInfo.print();
		}
	}



	// printf("Arming...\n");

	// int arm_state = dc_A.get_armed();

	

	// 	dc_A.send_arm_command(1);
	// 	// wait to get an ack
	// 	std::this_thread::sleep_for(std::chrono::microseconds(1000000));
	// 	arm_state = dc_A.get_armed();

	//////////////////////////IPERF COMMAND
	wrpr.iperf_start();


	std::cout<<GREEN<<"STARTING MISSION\n"<<RESET;
	
	wrpr.get_iperf_thread()->join();	

	// dc_A.mission_start();

	
	// std::this_thread::sleep_for(std::chrono::seconds(30));

	// drone_landed_state = dc_A.get_landed_state();
	
	// if(drone_landed_state==MAV_LANDED_STATE_ON_GROUND)
	// {
	// 	std::cout<<RED<<" Drone did not start PRESS q KEY TO END MISSION or c to continue\n"<<RESET;
	// 	c=getchar();
	// 	if(c=='q'){
	// 		dc_A.send_arm_command(0);
	// 		wrpr.iperf_stop();

	// 		dc_A.request_termination();
	// 		receiver_thread.join();
	// 		std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
	// 		return 0;
	// 	}
	// }

	// while (true)
	// {

	// 	drone_landed_state = dc_A.get_landed_state();
	// 	if(drone_landed_state==MAV_LANDED_STATE_ON_GROUND){
	// 		break;
	// 	}
	// 	else{
	// 		std::this_thread::sleep_for(std::chrono::seconds(5));
	// 	}
	// }

	


	// std::cout<<GREEN<<"DRONE LANDED\n"<<RESET;
	// dc_A.send_arm_command(0);
	// wrpr.iperf_stop();

	// dc_A.request_termination();
	// receiver_thread.join();
	// std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
	
	return(0);
}
