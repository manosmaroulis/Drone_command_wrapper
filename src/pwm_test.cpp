#include <unistd.h>
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#define SCALE_FACTOR 0.1
#define PWM_PIN 23
#define MAX_ANGLE 90
#define MAX_PWM 21
#define MAPPING_ANGLE_TO_PWM(X)((double(MAX_PWM)-double(SCALE_FACTOR)*(double(X))))


// #include <sys/stat.h>
// #include <string>
// #include<iostream>

int main(){


// std::string cmd;
// std::string server_ip;
// server_ip = "10.64.44.75";
// cmd ="ping -c1 -s1 "+server_ip+" > /dev/null 2>&1";


// int ret=1;

// do{

//     ret = system(cmd.c_str());
//     std::cout<<"eluses\n";
// }while (ret!=0);

	wiringPiSetup();

//	pinMode(PWM_PIN,PWM_OUTPUT);
	softPwmCreate(PWM_PIN,0,200);

    float angle = 0.0;
    int pwm_value=0;

    for(int i =0;i<18;i++){

	//pwm_value+=5;
        pwm_value = MAPPING_ANGLE_TO_PWM(angle);
	printf("angle is %f\n",angle);
        printf(" PWM %d\n",pwm_value);
	printf("-----------\n");
//        pwmWrite(PWM_PIN,pwm_value);
	softPwmWrite(PWM_PIN,pwm_value);
        angle+=5;
        sleep(2);
	//pwm_value+=1;
    }

}
