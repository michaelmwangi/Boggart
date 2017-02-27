#include "service.h"


Service::Service(std::string servicename){
    servicename_ = servicename;
    // initialize service IO worker thread
    std::thread t(std::bind(&Service::ConsumeRequest, this));
    t.detach();
}

void Service::ConsumeRequest(){
    while(true){
        std::string request = requests_.WaitAndPop();
        std::cout<<request<<std::endl;
    }
}

void Service::AddRequest(std::string workload){
    requests_.Push(workload);
}

void Service::AddWorker(std::shared_ptr<Worker> worker){
    workers_.push(worker);
}

int Service::TotalRequests(){
    return requests_.Size();
}
