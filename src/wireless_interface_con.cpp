#include "wireless_interface_con.h"

wireless_interface_con::wireless_interface_con(/* args */)
{
    ifaddr=nullptr;
    // wlan_name = nullptr;

}

wireless_interface_con::~wireless_interface_con()
{
    freeifaddrs(ifaddr);
}


void wireless_interface_con::find_interfaces(){
    int family, s;
    char host[NI_MAXHOST];
    //make sure to delete previous information
    //the first time this function is called it does not have any effect free(nullptr)
    freeifaddrs(ifaddr);
    wlan_names.clear();

    if (getifaddrs(&ifaddr) == -1) {
        std::cerr<<"getifaddrs";
        ifaddr = nullptr;
        return;
    }
    /* Walk through linked list, maintaining head pointer so we
        can free list later. */

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL;
            ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
            form of the latter for the common families). */

        // printf("%-8s %s (%d)\n",
        //         ifa->ifa_name,
        //         (family == AF_PACKET) ? "AF_PACKET" :
        //         (family == AF_INET) ? "AF_INET" :
        //         (family == AF_INET6) ? "AF_INET6" : "???",
        //         family);
  
     
        /* For an AF_INET* interface address, display the address. */
        // check only IPv4
        if (family == AF_INET /*|| family == AF_INET6*/) {
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                            sizeof(struct sockaddr_in6),
                    host, NI_MAXHOST,
                    NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                // printf("getnameinfo() failed: %s\n", gai_strerror(s));
                std::cout<<"getnameinfo() failed:"<< gai_strerror(s)<<"\n";
                return;
            }
            std::cout<<ifa->ifa_name<<"\n";

            wlan_names.push_back(ifa->ifa_name);
            std::cout<<"\t\taddress: <"<< host<<">\n";

        } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
           // struct rtnl_link_stats *stats =static_cast<rtnl_link_stats*>(ifa->ifa_data);

            // printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
            //         "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
            //         stats->tx_packets, stats->rx_packets,
            //         stats->tx_bytes, stats->rx_bytes);
        }
    }
}


int wireless_interface_con::getSignalInfo(signalInfo* sigInfo,char *iwname){

    iwreq req;
    strcpy(req.ifr_name, iwname);

    iw_statistics *stats;

    //have to use a socket for ioctl
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //make room for the iw_statistics object
    req.u.data.pointer = (iw_statistics *)malloc(sizeof(iw_statistics));
    req.u.data.length = sizeof(iw_statistics);

    //this will gather the signal strength
    if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1){
        //die with error, invalid interface
        std::cout<<"Invalid Interface\n";
        close(sockfd);
        return(-1);
    }
    else if(((iw_statistics *)req.u.data.pointer)->qual.updated & IW_QUAL_DBM){
        //signal is measured in dBm and is valid for us to use
        sigInfo->level=((iw_statistics *)req.u.data.pointer)->qual.level - 256;
    }

    //SIOCGIWESSID for ssid
    char buffer[32];
    memset(buffer, 0, 32);
    req.u.essid.pointer = buffer;
    req.u.essid.length = 32;
    //this will gather the SSID of the connected network
    if(ioctl(sockfd, SIOCGIWESSID, &req) == -1){
        //die with error, invalid interface
        return(-1);
    }
    else{
        memcpy(&sigInfo->ssid, req.u.essid.pointer, req.u.essid.length);
        memset(&sigInfo->ssid[req.u.essid.length],0,1);
    }

    //SIOCGIWRATE for bits/sec (convert to mbit)
    int bitrate=-1;
    //this will get the bitrate of the link
    if(ioctl(sockfd, SIOCGIWRATE, &req) == -1){
        std::cerr<<"bitratefall\n";
        return(-1);
    }else{
        memcpy(&bitrate, &req.u.bitrate, sizeof(int));
        sigInfo->bitrate=bitrate/1000000;
    }


    //SIOCGIFHWADDR for mac addr
    ifreq req2;
    strcpy(req2.ifr_name, iwname);
    //this will get the mac address of the interface
    if(ioctl(sockfd, SIOCGIFHWADDR, &req2) == -1){
        std::cerr<<"mac error\n";
        return(-1);
    }
    else{
        sprintf(sigInfo->mac, "%.2X", (unsigned char)req2.ifr_hwaddr.sa_data[0]);
        for(int s=1; s<6; s++){
            sprintf(sigInfo->mac+strlen(sigInfo->mac), ":%.2X", (unsigned char)req2.ifr_hwaddr.sa_data[s]);
        }
    }
    close(sockfd);
    return 1;
}

const std::vector<std::string>& wireless_interface_con::get_wlan_names(){

    return wlan_names;
}
