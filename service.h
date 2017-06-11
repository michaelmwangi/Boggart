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
    BlockingQueue<std::map<std::string, std::string>> jobs_; // holds the jobs to be processed by workers
    BlockingQueue<std::map<std::string, std::string>> async_job_results_; // holds the result of asynchronous job results
    BlockingQueue<std::map<std::string, std::string>> sync_job_results_; // // holds the result of synchronous job results

public:
    Service(std::string, std::string);
    void AddJob(std::string jobid, std::string workpayload);
    std::string GetResult(std::string jobid, bool sync=false);
    void AddWorker(std::shared_ptr<Worker>);
    int TotalRequests();
    void ConsumeJob();
};



#endif // SERVICE_H
