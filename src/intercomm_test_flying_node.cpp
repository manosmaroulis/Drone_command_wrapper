#include <thread>
#include "wireless_interface_con.h"
// to compile: g++ -I mavlink/include/mavlink/v2.0 class_rover_test.cpp serial_port.cpp rover.cpp -o class
 
// #include <sys/stat.h>
#include "iperf_wrapper.h"
#include "Drone_Command.h"
#include<math.h>



#include <wiringPi.h>
#include <softPwm.h>
// #include

static constexpr double CONSTANTS_RADIUS_OF_EARTH = 6371000;					// meters (m)
#define M_PI 3.141592653589793238462643383280

#define RAD_TO_DEG 57.2957
#define DEGREES_ERROR_THRESHOLD 5.0
#define MAPPING_CONSTANT 7.8
#define PWM_MIN 1300
#define MAX_ANGLE 90
// #define MAPPING_ANGLE_TO_PWM(X)(double(PWM_MIN)+(double(MAPPING_CONSTANT)*(double(MAX_ANGLE-X))))
#define SCALE_FACTOR 0.2222
#define PWM_PIN 23

#define MAPPING_ANGLE_TO_PWM(X)((double(SCALE_FACTOR)*(double(MAX_ANGLE-X))))


double radians(double degrees){

	return (degrees/double(180.0))*double(M_PI);
}

double wrap_pi(double x,double low,double high){

	 // already in range
    if (low <= x && x < high) {
        return x;
    }

    const auto range = high - low;
    const auto inv_range = float(1) / range; // should evaluate at compile time, multiplies below at runtime
    const auto num_wraps = floor((x - low) * inv_range);
    return x - range * num_wraps;
}

float get_distance_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next)
{
	const double lat_now_rad = radians(lat_now);
	const double lat_next_rad = radians(lat_next);

	const double d_lat = lat_next_rad - lat_now_rad;
	const double d_lon = radians(lon_next) - radians(lon_now);

	const double a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(lat_next_rad);

	const double c = atan2(sqrt(a), sqrt(1.0 - a));

	return static_cast<float>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

float get_bearing_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next)
{
	const double lat_now_rad = radians(lat_now);
	const double lat_next_rad =radians(lat_next);

	const double cos_lat_next = cos(lat_next_rad);
	const double d_lon = radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	const float y = static_cast<float>(sin(d_lon) * cos_lat_next);
	const float x = static_cast<float>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));
	
	return wrap_pi(atan2f(y, x),double(-M_PI),double(M_PI));
}



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

	dc_A.initialise_intercomm("192.168.200.10","192.168.200.1",14550,14551,false);


	wiringPiSetup();

	softPwmCreate(PWM_PIN,0,100);
	// softPwmCreate(26,0,100);


	
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


	mavlink_peer_position_t peer_pos;
	mavlink_global_position_int_t gps;
	float theta_prev = 0.0;
	int pwm_value=0;
	sleep(3);
	wrpr.iperf_start();

	while(1){
			//receive peer_gps_position
			peer_pos.static_pos= false;
			if(dc_A.get_icm_obj()->receive_gps_position(peer_pos)){
    			

				if(peer_pos.static_pos){
					//currently used to stop the system
					softPwmWrite(PWM_PIN,7);
					break;
				}
				dc_A.get_position(gps);

				float dist = get_distance_to_next_waypoint(gps.lat,gps.lon,peer_pos.lat,peer_pos.lon);
				printf("dist------------------ %f\n",dist);

				// float delta_yaw = get_bearing_to_next_waypoint(gps.lat,gps.lon,peer_pos.lat,peer_pos.lon);
				// if(delta_yaw>float(DEGREES_ERROR_THRESHOLD)){}
				if(dist ==0){
					continue;
				}
				float theta = atanf((gps.relative_alt-peer_pos.relative_alt)/dist)*float(RAD_TO_DEG);
				// float theta = wrap_pi(atanf((gps.relative_alt-peer_pos.relative_alt)/dist),double(-M_PI),double(M_PI))*float(RAD_TO_DEG);
				//dc_A.send_message(dc_A.get_system_id(),dc_A.get_component_id(),meas);			//send to FMU 

				if(abs(theta - theta_prev)> float(DEGREES_ERROR_THRESHOLD)){

					theta_prev = theta;
					float dth = MAX_ANGLE-theta_prev;

					if(dth > 0 && dth<DEGREES_ERROR_THRESHOLD){//
						pwm_value = 22;
					}else if(theta_prev>0 && theta_prev<10){
						pwm_value=7;
					}else{
						pwm_value = MAPPING_ANGLE_TO_PWM(double(theta_prev));
					}

					if(pwm_value<1 || pwm_value>25){
						printf("YOUR ALGORITHM IS WRONg\n");
					}else{
						//set pwm values
						softPwmWrite(PWM_PIN,pwm_value);
					}
					usleep(100000);
				}
            }
		
	}



	dc_A.request_termination();
	wrpr.iperf_stop();
	receiver_thread.join();

		// dc_A.request_termination();

	// if(c=='q'){
		// dc_A.send_arm_command(0);
		
		// std::cout<<GREEN<<"APPLICATION EXITING\n"<<RESET;
		// return 0;
	// }
 
    return 0;
}