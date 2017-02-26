#include "service.h"

Service::Service(std::string servicename){
    servicename_ = servicename;
}

void Service::AddWork(std::string workload){
    requests_.push(workload);
}

void Service::AddWorker(std::shared_ptr<Worker> worker){
    workers_.push(worker);
}

int Service::TotalWork(){
    return requests_.size();
}
