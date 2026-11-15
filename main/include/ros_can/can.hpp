#ifndef CAN_HPP
#define CAN_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <net/if.h>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

class can{
    public:

    sockaddr_can address_;
    string interface_;
    string txqueuelen_="1000";
    int can_;
    unsigned int index;
    struct can_frame frames_;
    struct can_frame rx_;
    std::mutex rx_mutex_;

    std::thread rx_thread_;
    can(string interface="can0"):interface_(interface)
    {
       
        string command1="sudo ip link set " + interface_ + " "+ "down";
        string command3="sudo ip link set " + interface_ + " " + "up " + "type can bitrate 1000000";
        string command2 = "sudo ip link set " + interface_ + " txqueuelen " + txqueuelen_;
        int system1=system(command1.c_str());
        int system2=system(command2.c_str());

        if((system1 || system2)==0)
        {
            std::cout<<"interface " + interface_ + " found and getting configured"<<std::endl;
        }

        else
        {
            std::cout<<"Failed to configure interface. Check sudo privileges or the CAN device"<<endl; 
            exit(EXIT_FAILURE);
        }


        if (system(command3.c_str()) != 0)
    std::cerr << "can: command3 failed" << std::endl;

    }
    void start_rx()
    {

        can_=socket(AF_CAN,SOCK_RAW,CAN_RAW);
        index=if_nametoindex(interface_.c_str());

        if(can_<0)
        {
            perror("cant setup the CAN");
            exit(EXIT_FAILURE);
        }
        
        address_.can_family=AF_CAN;
        address_.can_ifindex=index;

        if(bind(can_,reinterpret_cast<struct sockaddr*>(&address_),sizeof(address_))<0)
        {
            perror("Failed to bind ");
            exit(EXIT_FAILURE);
        }

        std::cout<<"Made connection"<<endl;

        rx_thread_ = std::thread(&can::read_data_loop, this);

    }

    void send_can(struct can_frame frames,bool extended=true)
    {
        
        frames_=frames;
        if (extended)
        {
            frames_.can_id = frames_.can_id | CAN_EFF_FLAG;
        }
        else
        {
            frames_.can_id = frames_.can_id & CAN_SFF_MASK;
        }

        ssize_t bytes=write(can_,&frames_,sizeof(frames_));
        if(bytes!=sizeof(frames_))
        {
            perror("Error sending data");
            
        }
        
    }

    void read_data_loop()
    {
        struct can_frame temp{};

        while (true)
        {
            ssize_t bytes = read(can_, &temp, sizeof(temp));

            if (bytes < 0)
            {
                perror("Data reading failed");
                continue;
            }

            if (bytes != sizeof(temp))
            {
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(rx_mutex_);
                rx_ = temp;
            }
        }
    }

    can_frame read_data()
    {
        std::lock_guard<std::mutex> lock(rx_mutex_);
        return rx_;
    }

    

};

#endif