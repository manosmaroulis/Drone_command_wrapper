#ifndef DRONE_COMMAND_H
#define DRONE_COMMAND_H



#ifdef USEDEBUG
#define Debug(COLOR,x,RESET) std::cout<<COLOR<< x<<RESET
#else
#define Debug(COLOR,x,RESET) 
#endif 






/////////////////////////////////// color codes
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//////////////////////////////


// Mavlink related library
#include "serial_port.h"
#include <common/mavlink.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include<atomic>

// Mavlink additional define
#define GUIDED 15
#define ARMED 128
#define HOLD 17

// BAUDRATE and port hard setup:
#define BAUDRATE 57600
#define UARTNAME "/dev/ttyACM1"
#define UARTNAME_2 "/dev/ttyACM0"



//message wrapper
struct message_result{

	// message 0, command 1
	//Currently messages are ignored
	uint64_t message_num;
	uint64_t command_number;
	// 0 means accepted and executed, 1 means temporary rejected/denied
	uint64_t command_executed;
	
	message_result(){};
	message_result(uint64_t msg_num,uint64_t com_num,uint64_t exec){
		message_num = msg_num;
		command_number = com_num;
		command_executed = exec;
	}

	message_result(const message_result &other){
		message_num = other.message_num;
		command_number = other.command_number;
		command_executed = other.command_executed;
	}
	message_result& operator=(const message_result &other){
		message_num = other.message_num;
		command_number = other.command_number;
		command_executed = other.command_executed;
		return *this;
	}
};


class Drone_Command
{
	private:

		int r_baudrate;

		char *r_uart_name;

		Serial_Port serial_port;

		// ARMED STATE, DISARMED =0, ARMED =1
		uint8_t r_armed;

		// DRONE CURRENT MODE
		uint8_t r_mode;

		//DRONE LANDED STATE
		// int landed_state;
		std::atomic<int>landed_state;

		int system_id;
		// COMPONENT TO WHICH A MSG/CMD IS ADDRESSED TO, broadcast = 0
		int component_id;

		// COMPANION COMPUTER ID
		int companion_id;

		// TERMINATION FLAG, USED BY THREADS
		bool is_not_end;

		// VARIABLE MUTEX
		std::mutex mtx;

		// OPERATION MUTEX (CURRENTLY SERIAL READ/WRITE)
		std::mutex rw_mtx;

		//file descriptor for logging
		std::ofstream gps_file;



	public:
		// Constructor:
		Drone_Command();

		~Drone_Command();
		//WAIT FOR A HEARTBIT BEFORE PROCEEDING TO ANY ACTIVITY
		void get_heartbit();
		
		// Get the current mode of the Vehicle:
		int get_mode();
		
		// Get armed state of the Vehicle:
		int get_armed();


		// Get land state of drone air/ground/landig/taking off

		int get_landed_state();
		
		//USED INTERNALLY TO CHANGE STATE, USER SHOULD USE send_arm_command(int state)
		void arm(bool state); //true is armed false is disarmed
		
		// Handle message from FMU:
		message_result handle_message(mavlink_message_t *msg);
		

	
		// Receive message:
		message_result recv_data();
		
		//Start an autonomous mission, the plan should be already uploaded
		int mission_start();

		int continue_mission();

		// GIVE PRIVILEGES FOR COMPANION COMPUTER COMMANDS, does not work 
		int toggle_offboard_control(bool flag);		
		
		// UNUSED COMMAND
		int test_command();
		
		// STOP AND HOVER/LOITER TO A CERTAIN WAYPOINT
		int hold_to_waypoint();

		// TRY TO ARM THE VEHICLE 
		int send_arm_command(int state);

		// ENABLE DATA STREAM FOR A SPECIFIC MSG, currently used for GPS readings
		int enable_msg_interval(int msg_id);

		// to do add more param arguments
		int send_command(uint8_t target_system,uint8_t target_component,uint16_t command,bool confirmation,float param1=-1,float param2=-1);

		// CHECK IF TERMINATION IS REQUESTED FROM ANOTHER THREAD
		bool termination_is_requested();

		void request_termination();

		//READ FROM SERIAL
		bool serial_read(mavlink_message_t&);
		
		// void run();
				
};



#endif
