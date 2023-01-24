// #include<iostream>
// #include<string.h>
#ifndef INTERCOMMUNICATION_H
#define INTERCOMMUNICATION_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h> /* required for the definition of bool in C99 */
#endif

#include <common/mavlink.h>
#include<common/mavlink_msg_peer_position.h>

// #include "Drone_Command.h"

#define BUFFER_LENGTH 2041 // minimum buffer size that can be used with qnx (I don't know why)


class Intercommunication
{
private:

    int32_t position[8];
    int sock;
    struct sockaddr_in gcAddr; 
	struct sockaddr_in locAddr;
	//struct sockaddr_in fromAddr;
	uint8_t send_buf[BUFFER_LENGTH];
    uint8_t rec_buf[BUFFER_LENGTH];
	socklen_t fromlen;
	uint16_t len;
    char target_ip[100];


public:
    Intercommunication(char*, char*,int,int);
    ~Intercommunication();

    uint64_t microsSinceEpoch();
    //TODO receive position from gps sensor
    void get_gps_position();

    // It can be extended to whatever message we want, for this project we nee gps
    int send_gps_position(int target_system,int target_component,mavlink_global_position_int_t, bool static_position);
    int receive_gps_position(mavlink_peer_position_t* gps);
};

#endif