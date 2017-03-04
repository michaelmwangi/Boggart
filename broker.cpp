#include "broker.h"

Broker::Broker(std::string qhost, std::string qport){
    host_ = qhost;
    port_ = qport;
    endpoint_url_ = qhost + ":" + qport;
}

void Broker::StartBroker(){
    zmqpp::context context;
    zmqpp::poller poller;
    server_= std::make_unique<zmqpp::socket>(context, zmqpp::socket_type::router);
//    zmqpp::socket server_->_->(context, zmqpp::socket_type::router);

    server_->bind(endpoint_url_);
    poller.add(*server_.get());

    while(true){
        bool has_items = poller.poll();
        if (has_items){
            zmqpp::message msg;
            server_->receive(msg);
            if (msg.parts() <= 0 ){
                server_->send("Number of arguments not satisfiable");
            }
            else{
                std::string sender_addr = msg.get(0);
                std::string empty = msg.get(1);
                std::string signature = msg.get(2);

                if(signature.compare("BOGC01") == 0){
                    // handle client request
                    if(msg.parts() >= 5){
                        std::string service_name = msg.get(3);
                        std::string payload = msg.get(4);
                        HandleClient(service_name, payload);
                    } else{
                        server_->send("Unsatisfiable number of arguments passed");
                    }

               }else if(signature.compare("BOGW01") == 0){
                    // handle worker requests
                    std::cout<<"Handling worker request "<< msg.parts()<<std::endl;
                    std::string service_name = msg.get(4);
                    HandleWorker(msg.copy());
               }else if(signature.compare("BOGI01") == 0){
                    std::cout<<"Am here "<<std::endl;
                    HandleInternalCommunication(msg.copy());
               }else{
                    server_->send("Unknown signature passed");
                }
            }
        }
    }
}

std::shared_ptr<Service> Broker::GetService(std::string service_name){
    std::map<std::string, std::shared_ptr<Service>>::iterator it = services_.find(service_name);
    if (it == services_.end()){
        auto service = std::make_shared<Service>(service_name, port_);
        services_.insert(std::make_pair(service_name, service));
        std::cout<<"Created a new service "<<service_name<<std::endl;        
        return service;
    }else{
        return services_[service_name];
    }
}

void Broker::HandleClient(std::string service_name, std::string payload){
    std::shared_ptr<Service> service = GetService(service_name);
    service->AddRequest(payload);
    std::cout<<"Service "<<service_name<<" has "<< services_[service_name]->TotalRequests()<<" Items"<<std::endl;
}

void Broker::HandleWorker(zmqpp::message msg){
    std::string worker_addr = msg.get(0);
    std::string worker_cmd = msg.get(3);
    std::string service_name = msg.get(4);
    std::shared_ptr<Service> service = GetService(service_name);
    std::shared_ptr<Worker> worker = std::make_shared<Worker>(worker_addr);
    if (worker_cmd == "READY"){
        // check msg parts requirements
        service->AddWorker(worker);
    } else if(worker_cmd == "WORKER_RESP"){
        // check msg parts requirements
        std::string resp = msg.get(5);
        std::cout<<"Worker resp "<<resp<<std::endl;
        service->AddWorker(worker);
    }

}

void Broker::HandleInternalCommunication(zmqpp::message msg){
    std::string type = msg.get(3);
    std::cout<<"Type is "<<type<<std::endl;
    if(type == "WORK"){
        // we are pushing work to the worker
        std::string worker_addr = msg.get(4);
        std::string work_payload = msg.get(5);
        zmqpp::message work;
        work.push_back(worker_addr);
        work.push_back(work_payload);
        std::cout<<"Sending work to worker "<<worker_addr<<std::endl;
        server_->send(work);
    }
}
