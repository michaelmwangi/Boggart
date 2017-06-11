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
#include <stdexcept>
#include <unordered_map>
#include "opdefinitions.h"
#include "boggartclient.h"
#include "rapidjson/document.h"


#define READ_BUF_SIZE (1024*16) // arbitrary buf read size 16 kb  

class Network{
private:
    std::string host_;
    std::string port_;
    int server_fd_;
    std::unordered_map<int, BoggartClient> boggart_clients_;
    void ProcessClientData(std::string payload, std::string service, int fd);
    void ProcessIncomingData(const char *payload, int fd);
    void AddConnection(int filedescriptor);
    void RemoveConnection(int filedescriptor);
    void CreateService(std::string servicename);
public:
    Network(std::string host, std::string port);
    void run();    
};

#endif 
