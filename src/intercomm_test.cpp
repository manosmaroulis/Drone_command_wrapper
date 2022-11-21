#include "Intercommunication.h"




int main(int argc,char* argv[]){

	Intercommunication icm("10.64.44.75","10.64.44.71");
    sleep(10);

    for(int i =0;i<10;i++){

    
        icm.send_gps_position();
        icm.receive_gps_position();
    }
    return 0;
}