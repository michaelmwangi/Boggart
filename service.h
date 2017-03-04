#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <functional>
#include <zmqpp/zmqpp.hpp>
#include "blockingqueue.h"


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
    std::string broker_port_;
    BlockingQueue<std::shared_ptr<Worker>> workers_;
    BlockingQueue<std::string> requests_;

public:
    Service(std::string, std::string);
    void AddRequest(std::string);
    void AddWorker(std::shared_ptr<Worker>);
    int TotalRequests();
    void ConsumeRequest();
};



#endif // SERVICE_H
