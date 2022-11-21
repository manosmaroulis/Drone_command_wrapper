#include "iperf_wrapper.h"



long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}





iperf_wrapper::iperf_wrapper(std::string file_name,bool server,std::string server_ip_,std::string client_ip_,std::string bandwidth):
iperf_file_name(file_name),bw(bandwidth),server_ip(server_ip_),client_ip(client_ip_)
{

    if(server){
        command = "iperf -s -u -i 1 -t 90> "+file_name;
    }else{

        command = "iperf -c "+server_ip_+" -u  -b "+bandwidth+"M -t 10000 -i 1 >"+file_name;
    }

}


iperf_wrapper::~iperf_wrapper(){}


std::shared_ptr<std::thread> iperf_wrapper::get_iperf_thread(){



    return iperf_thread;
}


void iperf_wrapper::iperf_start(){

    long starting_len;
    std::string cmd;

    if(is_server)
        cmd="ping -c1 -s1 "+client_ip+" > /dev/null 2>&1";
    else
        cmd="ping -c1 -s1 "+server_ip+" > /dev/null 2>&1";


    int ret=1;

    do{

        ret = system(cmd.c_str());
        
    }while (ret!=0);
  

    // int ret = system("ping -c1 -s1 8.8.8.3  > /dev/null 2>&1");
    
    iperf_thread = std::shared_ptr<std::thread>(new std::thread([&](){
        //starts a new process.
        system(this->command.c_str());
        std::cout<<"IPERF STOPPED\n";

    }));

    // std::this_thread::sleep_for(std::chrono::microseconds(10000));
    // //wait for the command to start
    
    
    // starting_len=GetFileSize(iperf_file_name);
    // std::cout<<"start_len "<< starting_len<<"\n";
    // assert(starting_len!=-1);
    // while (starting_len<MIN_LEN)
    // {
    //     starting_len=GetFileSize(iperf_file_name);
    //     assert(starting_len!=-1);

    // }

    // //wait for message to arrive
    // while(true){
    //     if(GetFileSize(iperf_file_name)>starting_len)
    //         break;

    //     assert(starting_len!=-1);
    // }
    
    std::cout<<"IPERF STARTED\n";

}


void iperf_wrapper::iperf_stop(){
    //end the iperf process
    // system("kill -9 $(ps aux | grep 'iperf' | awk '{print $2}')");
    system("pkill iperf");
    iperf_thread->join();
    std::cout<<"Thread finished\n";

}

