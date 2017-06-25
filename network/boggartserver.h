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
#include <memory>
#include <unordered_map>
#include <sys/time.h>
#include "rapidjson/document.h"
#include "opdefinitions.h"
#include "boggartworker.h"
#include "boggartclient.h"
#include "service.h"


#define READ_BUF_SIZE (1024*16) // arbitrary buf read size 16 kb  

class BoggartServer{
private:
    std::string host_;
    std::string port_;
    int server_fd_;
    std::unordered_map<std::string, std::shared_ptr<Service>> services_;
    std::shared_ptr<Service> GetService(std::string servicename);
    void ProcessClientData(std::string payload, std::string service, int fd);
    void ProcessWorkerData(std::string payload, std::string servicename, std::string command, int fd);
    void ProcessIncomingData(const char *payload, int fd);
    void PurgeWorkers();
    void CreateService(std::string servicename);
public:
    BoggartServer(std::string host, std::string port);
    void run();    
};

#endif 
