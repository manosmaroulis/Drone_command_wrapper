#include<Intercommunication.h>
Intercommunication::Intercommunication(char* myIP, char*peerIP,int myPort,int peerPort)
{

    sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    fromlen= sizeof(gcAddr);
    strcpy(target_ip,peerIP);

    memset(&locAddr, 0, sizeof(locAddr));
    locAddr.sin_family = AF_INET;   
	locAddr.sin_addr.s_addr = inet_addr(myIP);          //INADDR_ANY;
	locAddr.sin_port = htons(myPort);
    if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))
    {
		perror("error bind failed");
		close(sock);
		exit(EXIT_FAILURE);
    } 
    #if (defined __QNX__) | (defined __QNXNTO__)
        if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
    #else
        if (fcntl(sock, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
    #endif
    {
		fprintf(stderr, "error setting nonblocking: %s\n", strerror(errno));
		close(sock);
		exit(EXIT_FAILURE);
    }
    memset(&gcAddr, 0, sizeof(gcAddr));
	gcAddr.sin_family = AF_INET;
	gcAddr.sin_addr.s_addr = inet_addr(target_ip);
	gcAddr.sin_port = htons(peerPort);


    //position[0] = //latitude
    //position[1] = //longtitude
    //position[2] = //altitude
    //position[7] = //yaw

}

Intercommunication::~Intercommunication()
{
    close(sock);

}

int Intercommunication::send_gps_position(int target_system,int target_component,mavlink_global_position_int_t meas, bool static_position){
	mavlink_message_t msg;


    // position[7]= 10; //hdg (yaw)
    // position[0] = 20;
    // position[1] = 30;
    // position[2] = 40;
    // position[3] = 50;
    // position[4] = 60;
    // position[5] =70;
    // position[6]=1.2;


    // position[0] = meas.lat;
    // position[1] = meas.lon;
    // position[2] = meas.alt;
    // position[3] = meas.relative_alt;
    // position[4] = meas.vx;
    // position[5] =meas.vy;
    // position[6]=meas.vz;
    // position[7]= meas.hdg; //hdg (yaw)
    // meas.time_boot_ms;
    mavlink_msg_peer_position_pack(target_system,target_component,&msg,microsSinceEpoch(),1,static_position,meas.lat,meas.lon,meas.alt,meas.relative_alt,meas.hdg);
    // mavlink_msg_global_position_int_pack(1,200,&msg,microsSinceEpoch(),position[0], position[1], position[2],
	// 									position[3], position[4], position[5],position[6],position[7]);


    len = mavlink_msg_to_send_buffer(send_buf,&msg);
    int bytes_sent = sendto(sock,send_buf,len,0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr_in));
    memset(send_buf, 0, BUFFER_LENGTH);

    return bytes_sent;
    
}

int Intercommunication::receive_gps_position(mavlink_peer_position_t& gps){
    int recsize =recvfrom(sock,(void *)rec_buf, BUFFER_LENGTH, 0, (struct sockaddr *)&gcAddr, &fromlen);

	unsigned int temp = 0;

    if(recsize>0){
        mavlink_message_t msg;

        mavlink_status_t status;
		printf("Bytes Received: %d\nDatagram: ", (int)recsize);

        for (int i = 0; i < recsize; ++i)
        {
            temp = rec_buf[i];
            printf("%02x ", (unsigned char)temp);
            if (mavlink_parse_char(MAVLINK_COMM_0, rec_buf[i], &msg, &status))
            {
                // Packet received
                printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
            }
        }

        // dc.send_command(dc.system_id,dc.component_id,0);
        // mavlink_msg_global_position_int_decode(&msg,&gps);
         mavlink_msg_peer_position_decode(&msg,&gps);
        //send message sends only gps positions FIX IT
        // dc.send_message(dc.get_system_id(),dc.get_component_id(),gps);

       
        // printf("\nGPS POSITION RECEIVED lat: %d lon: %d alt: %d hdg: %d\n",gps.lat,gps.lon,gps.alt,gps.hdg);
        // printf("\n %d %d %d %d\n",gps.relative_alt,gps.time_boot_ms,gps.vx,gps.vy,gps.vz);
    }
    memset(rec_buf, 0, BUFFER_LENGTH);
    sleep(1);
    return recsize;
}


/* QNX timer version */
#if (defined __QNX__) | (defined __QNXNTO__)
uint64_t Intercommunication::microsSinceEpoch()
{
	
	struct timespec time;
	
	uint64_t micros = 0;
	
	clock_gettime(CLOCK_REALTIME, &time);  
	micros = (uint64_t)time.tv_sec * 1000000 + time.tv_nsec/1000;
	
	return micros;
}
#else
uint64_t Intercommunication::microsSinceEpoch()
{
	
	struct timeval tv;
	
	uint64_t micros = 0;
	
	gettimeofday(&tv, NULL);  
	micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
	
	return micros;
}
#endif