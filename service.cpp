#include "service.h"


Service::Service(std::string servicename, std::string port){
    servicename_ = servicename;
    broker_port_ = port;
    std::thread t(std::bind(&Service::ConsumeRequest, this));
    t.detach();
}

void Service::ConsumeRequest(){
    zmqpp::context context;
    zmqpp::socket socket(context, zmqpp::socket_type::dealer);
    std::string endpoint = "tcp://localhost:"+broker_port_;
    socket.connect(endpoint);
    while(true){
        // grab worker first before we start consuming
        std::shared_ptr<Worker> worker = workers_.WaitAndPop();
        std::string work_payload = requests_.WaitAndPop();
        zmqpp::message msg;
        msg.push_back("");
        msg.push_back("BOGI01"); // internal service signature
        msg.push_back("WORK");
        msg.push_back(worker->address);
        msg.push_back(work_payload);
        socket.send(msg);
    }
}

void Service::AddRequest(std::string workload){
    requests_.Push(workload);
}

void Service::AddWorker(std::shared_ptr<Worker> worker){
    workers_.Push(worker);
}

int Service::TotalRequests(){
    return requests_.Size();
}
