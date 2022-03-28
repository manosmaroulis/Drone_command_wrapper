
#ifndef IPERF_WRAPPER
#define IPERF_WRAPPER
#include <string>
#include <sys/stat.h>
#include <thread>
#include<iostream>
#include <assert.h>
// #define NDEBUG
#define MIN_LEN 200

class iperf_wrapper
{
private:
    std::string iperf_file_name;
    bool is_server;
    std::string command;
    pid_t pid;
    std::unique_ptr<std::thread> iperf_thread;
    // std::string server_ip;
    /* data */
public:
    iperf_wrapper(std::string file_name,bool server,std::string server_ip);


    ~iperf_wrapper();

    // iperf start is blocking until there is a connection
   void iperf_start();
    void iperf_stop();
};


#endif