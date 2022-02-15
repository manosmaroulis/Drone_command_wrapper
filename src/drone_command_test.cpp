#include "Drone_Command.h"
#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 

int main()
{
	Drone_Command dc_A;
	wireless_interface_con wic;
	signalInfo sigInfo;

	

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
	int num_tries =0;
	int arm_state = dc_A.get_armed();

	

	while(arm_state!=1 && num_tries!=3){
		// arm(1);
		dc_A.send_arm_command(1);
		// wait to get an ack
		std::this_thread::sleep_for(std::chrono::microseconds(10000));
		arm_state = dc_A.get_armed();
		num_tries++;
	}

	printf("Starting mission...\n");
	
	int counter =0;
	while(counter<5){
		dc_A.mission_start();
		counter++;
	}
	// getchar();
	// counter =0;
	// while(counter<3){
	// 	dc_A.hold_to_waypoint();

	// 	counter++;
	// }

	dc_A.request_termination();
	receiver_thread.join();
	
	return(0);
}
