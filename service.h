#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <functional>
#include "requestsqueue.h"

struct Worker{
    std::string address; // address to use when routing messages
    int expiry; // time to expire before heartbeat

    Worker(std::string addr, int expire=0){
        address = addr;
        expiry = expire;
    }
};


class Service{
private:
    std::string servicename_;
    std::queue<std::shared_ptr<Worker>> workers_;
    RequestsQueue requests_;
    void ConsumeRequest();
public:
    Service(std::string);
    void AddRequest(std::string);
    void AddWorker(std::shared_ptr<Worker>);
    int TotalRequests();
};



#endif // SERVICE_H
