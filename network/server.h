#ifndef SERVER_H
#include "netdefinitions.h"
#include <string>

class Server{
private:    
    std::string ip_addr;
    int port;
    int server_fd;    
public:
    Server(std::string ip, int port);
    void start();
};
#endif