#include "service.h"

Service::Service(std::string servicename){
    servicename_ = servicename;
}

void Service::AddWork(std::string workload){
    requests_.push_back(workload);
}

int Service::TotalWork(){
    return requests_.size();
}
