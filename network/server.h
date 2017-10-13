#ifndef SERVER_H
#include "netdefinitions.h"
#include <string>

struct client{
    uint8_t data[BUF_SIZE];
    std::string cmd;
    int buf_end;
    int buf_beg;
};

class Server{
private:    
    std::string ip_addr;
    int port;
    int server_fd;
    client clients[MAXFDS];
public:
    Server(std::string ip, int port);
    void start();
};
#endif