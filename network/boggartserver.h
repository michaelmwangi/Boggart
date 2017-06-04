#ifndef BOGGART_SERVER_H
#define BOGGART_SERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include "opdefintions.h"
#include "rapidjson/document.h"


#define READ_BUF_SIZE (1024*16) // arbitrary buf read size 16 kb

class Network{
private:
    std::string host_;
    std::string port_;
    int server_fd_;
    void ProcessIncomingData(const char *payload);
public:
    Network(std::string, std::string);
    void run();    
};

#endif 
