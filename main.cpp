#include <iostream>
#include "network/server.h"

int main(int argc, char **argv){
    std::cout<<"Starting ..."<<std::endl;
    if(argc < 3){
        std::cout<<"Usage: ./boggart <ip_addr> <port>"<<std::endl;
    }else{
        Server boggart_s(argv[1], std::stoi(argv[2]));
        boggart_s.start();
    }

    return 0;
}