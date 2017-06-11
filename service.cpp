#include "service.h"


Service::Service(std::string servicename, std::string port){
    servicename_ = servicename;
    broker_port_ = port;
    std::thread t(std::bind(&Service::ConsumeJob, this));
    t.detach();
}

/**
 * @brief Service::ConsumeJob
 * This service worker thread uses this function to consume jobs from the queue and ship them
 * to the workers
 */
void Service::ConsumeJob(){

    while(true){
        // grab worker first before we start consuming
        std::shared_ptr<Worker> worker = workers_.WaitAndPop();
        std::map<std::string, std::string> work_payload = requests_.WaitAndPop();
        std::string job_id;
        std::string job;
        for (auto &x: work_payload){
            // only has one mapping
            job_id = x.first;
            job = x.second;
        }

    }
}

void Service::AddJob(std::string jobid, std::string workpayload){
    std::map<std::string, std::string> work = {{jobid, workpayload}};
    jobs_.Push(work);
}

void Service::AddWorker(std::shared_ptr<Worker> worker){
    workers_.Push(worker);
}

int Service::TotalRequests(){
    return requests_.Size();
}
