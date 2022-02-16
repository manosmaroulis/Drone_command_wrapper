#include "Drone_Command.h"
#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 
#include <sys/stat.h>

#define MIN_LEN 200

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


int main()
{
	Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;
	std::string iperf_file_name = "iperf.txt";

	

	printf("Waiting for heartbit\n");
	dc_A.get_heartbit();

	//enable gps readings
	dc_A.enable_msg_interval(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
	
	/*Start a background thread to receive messages*/
	std::thread receiver_thread([&](){
		while(!dc_A.termination_is_requested()){
			dc_A.recv_data();
			std::this_thread::sleep_for(std::chrono::microseconds(5000));
		}
	});


	//CHECK WIFI INTERFACES FOR A CONNECTION
	wic.find_interfaces();
	auto names = wic.get_wlan_names();

	for(auto& name: names){
		char* c_name = &name[0];
		int result = wic.getSignalInfo(&sigInfo,c_name);

		if(result==1){
			printf("Connection established");
			sigInfo.print();
		}
	}



	printf("Arming...\n");
	// int num_tries =0;
	int arm_state = dc_A.get_armed();

	

	while(arm_state!=1 /*&& num_tries!=3*/){
		// arm(1);
		dc_A.send_arm_command(1);
		// wait to get an ack
		std::this_thread::sleep_for(std::chrono::microseconds(10000));
		arm_state = dc_A.get_armed();
		// num_tries++;
	}

	//////////////////////////IPERF COMMAND
	long starting_len = GetFileSize(iperf_file_name);
	//make sure to get past initialization bytes
    while(starting_len<MIN_LEN){
        starting_len = GetFileSize(iperf_file_name);
    }

	// When the iperf starts file size will get bigger
    while(true){
        
        long len = GetFileSize(iperf_file_name);
        // cout<<len<<"\n";
        
        if(len>starting_len)
            break;

		std::this_thread::sleep_for(std::chrono::microseconds(5000));
    }

	std::cout<<GREEN<<"IPERF COMMAND STARTED\n"<<RESET;
	///////////////////////////////////////////////////


	printf("Starting mission...\n");
	
	// int counter =0;
	// while(counter<5){
		dc_A.mission_start();
		// counter++;
	// }

	getchar();
	// counter =0;
	// while(counter<3){
	// 	dc_A.hold_to_waypoint();

	// 	counter++;
	// }

	dc_A.request_termination();
	receiver_thread.join();
	
	return(0);
}
