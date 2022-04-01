#include "Drone_Command.h"
#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 
// #include <sys/stat.h>
#include "iperf_wrapper.h"
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

	
	bool is_server;
	std::string iperf_file;
	std::string server_ip;
	std::string bw;
	
	if(argc<3){
		std::cout<<RED<<"TOO FEW ARGUMENTS\n"<<RESET;
		return -1;
	}else{

		iperf_file = std::string(argv[1]);
		is_server= std::string(argv[2])=="1"?true:false;
		server_ip=std::string(argv[3]);
		bw =std::string(argv[4]);
	}
	
	char c;
	Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;
	int drone_landed_state = 0;


	// std::string iperf_file_name = std::string(argv[1]);


	iperf_wrapper wrpr(iperf_file,is_server,server_ip,bw);
	
	dc_A.enable_msg_interval(MAVLINK_MSG_ID_HEARTBEAT);
	
	// MAVLINK_MSG_ID_EXTENDED_SYS_STATE
	printf("Waiting for heartbit\n");
	dc_A.get_heartbit();

	
	/*Start a background thread to receive messages*/

	std::thread receiver_thread([&](){
		while(!dc_A.termination_is_requested()){
			dc_A.recv_data();
			// std::this_thread::sleep_for(std::chrono::microseconds(5000));
		}
	});


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



	printf("Arming...\n");

	int arm_state = dc_A.get_armed();

	

	// while(arm_state!=1 ){
		dc_A.send_arm_command(1);
		// wait to get an ack
		std::this_thread::sleep_for(std::chrono::microseconds(1000000));
		arm_state = dc_A.get_armed();
	// }

	//////////////////////////IPERF COMMAND
	wrpr.iperf_start();
	dc_A.enable_msg_interval(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);


	std::cout<<GREEN<<"STARTING MISSION\n"<<RESET;
	
	
	

	dc_A.mission_start();



	// std::cout<<RED<<"PRESS q KEY TO END MISSION\n"<<RESET;
	
	// c=getchar();
	// while(c!='q'){
	// 	c=getchar();
	// }

	
	std::this_thread::sleep_for(std::chrono::seconds(10));

	drone_landed_state = dc_A.get_landed_state();
	
	if(drone_landed_state==MAV_LANDED_STATE_ON_GROUND)
	{
		std::cout<<RED<<" Drone did not start PRESS q KEY TO END MISSION or c to continue\n"<<RESET;
		c=getchar();
		if(c=='q'){
			dc_A.send_arm_command(0);
			wrpr.iperf_stop();

			dc_A.request_termination();
			receiver_thread.join();
			std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
			return 0;
		}
	}
	// if(drone_landed_state==MAV_LANDED_STATE_TAKEOFF||drone_landed_state==MAV_LANDED_STATE_IN_AIR)

	while (true)
	{

		drone_landed_state = dc_A.get_landed_state();
		if(drone_landed_state==MAV_LANDED_STATE_ON_GROUND){
			break;
		}
		else{
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}

	


	std::cout<<GREEN<<"DRONE LANDED\n"<<RESET;
	dc_A.send_arm_command(0);
	wrpr.iperf_stop();

	dc_A.request_termination();
	receiver_thread.join();
	std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
	
	return(0);
}
