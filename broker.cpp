#include "broker.h"

Broker::Broker(std::string qhost, std::string qport){
    host_ = qhost;
    port_ = qport;
    endpoint_url_ = qhost + ":" + qport;
}

void Broker::StartBroker(){
    zmqpp::context context;
    zmqpp::poller poller;
    zmqpp::socket socket(context, zmqpp::socket_type::router);
    socket.bind(endpoint_url_);
    poller.add(socket);

    while(true){
        bool has_items = poller.poll();

        if (has_items){
            zmqpp::message msg;
            socket.receive(msg);
            if (msg.parts() <= 0 ){
                socket.send("Number of arguments not satisfiable");
            }
            else{
                std::string sender_addr = msg.get(0);
                std::string empty = msg.get(1);
                std::string signature = msg.get(2);
                std::string service_name = msg.get(3);

                if(signature.compare("BOGC01") == 0){
                    // handle client request
                    std::string payload = msg.get(4);
                    HandleClient(service_name, payload);
               }else if(signature.compare("BOGW01") == 0){
                    // handle worker requests
                    std::cout<<"Handling worker request "<< msg.parts()<<std::endl;
                    HandleWorker(service_name, sender_addr);
               }else{
                    socket.send("Unknown signature passed");
                }
            }
        }
    }
}

std::shared_ptr<Service> Broker::GetService(std::string service_name){
    std::map<std::string, std::shared_ptr<Service>>::iterator it = services_.find(service_name);
    if (it == services_.end()){
        auto service = std::make_shared<Service>(service_name);
        services_.insert(std::make_pair(service_name, service));
        std::cout<<"Created a new service "<<service_name<<std::endl;
        return service;
    }else{
        return services_[service_name];
    }
}

void Broker::HandleClient(std::string service_name, std::string payload){
    std::shared_ptr<Service> service = GetService(service_name);
    service->AddWork(payload);
    std::cout<<"Service "<<service_name<<" has "<< services_[service_name]->TotalWork()<<" Items"<<std::endl;
}

void Broker::HandleWorker(std::string service_name, std::string addr){
    std::shared_ptr<Service> service = GetService(service_name);
    std::shared_ptr<Worker> worker = std::make_shared<Worker>(addr);
    service->AddWorker(worker);
}
