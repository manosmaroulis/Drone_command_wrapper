#include "iperf_wrapper.h"



long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}





iperf_wrapper::iperf_wrapper(std::string file_name,bool server,std::string server_ip):
iperf_file_name(file_name)
{

    if(server){
        command = "iperf -s -u -i 1 > "+file_name;
    }else{
        command = "iperf -c "+server_ip+" -u  -b 10M -t 10000 -i 1 >"+file_name;
    }

}


iperf_wrapper::~iperf_wrapper(){}

void iperf_wrapper::iperf_start(){

    long starting_len;

    iperf_thread = std::unique_ptr<std::thread>(new std::thread([&](){
        //starts a new process.
        system(this->command.c_str());
        std::cout<<"IPERF STOPPED\n";

    }));

    std::this_thread::sleep_for(std::chrono::microseconds(10000));
    //wait for the command to start
    starting_len=GetFileSize(iperf_file_name);
    assert(starting_len!=-1);
    while (starting_len<MIN_LEN)
    {
        starting_len=GetFileSize(iperf_file_name);
        assert(starting_len!=-1);

    }

    //wait for message to arrive
    while(true){
        if(GetFileSize(iperf_file_name)>starting_len)
            break;

        assert(starting_len!=-1);
    }
    
    std::cout<<"IPERF STARTED\n";

}


void iperf_wrapper::iperf_stop(){
    //end the iperf process
    system("kill -9 $(ps aux | grep 'iperf' | awk '{print $2}')");
    iperf_thread->join();

}

