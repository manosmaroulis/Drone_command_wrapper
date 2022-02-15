#ifndef  WIRELESS_INTERFACE_CON_H
#define WIRELESS_INTERFACE_CON_H
#include<cstring>
#include<linux/wireless.h>
#include<sys/ioctl.h>
#include <unistd.h>
// #include <stdlib.h>
// #include <linux/if_link.h>
// #include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include<iostream>
#include <vector>


struct signalInfo {
    char mac[18];
    char ssid[33];
    int bitrate;
    int level;

    signalInfo(){
        memset(mac,'0',18);
        memset(ssid,'0',33);
        bitrate =-1;
        level = 0;

    }

    void print(){
        std::cout<<"MAC "<<mac<<"\n";
        std::cout <<"SSID "<<ssid<<"\n";
        std::cout <<"bitrate "<<bitrate<<"\n";
        std::cout <<"level "<<level<<"\n";
    }

};

class wireless_interface_con
{
private:
    /* data */
    // signalInfo siginf;
    // char* wlan_name;
    struct ifaddrs *ifaddr;
    std::vector<std::string> wlan_names;


public:
    wireless_interface_con(/* args */);
    ~wireless_interface_con();
   
    // list of all available interfaces
    const std::vector<std::string>& get_wlan_names();
    
    // find available network interfaces 
    void find_interfaces();
    
    // check if there is a connection for iwname interface
    int getSignalInfo(signalInfo* sigInfo,char* iwname);
};


#endif
