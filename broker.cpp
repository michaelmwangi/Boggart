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
            if (msg.parts()!= 5 ){
                socket.send("Number of arguments not satisfiable");
            }
            else{
                std::string sender_addr = msg.get(0);
                std::string empty = msg.get(1);
                std::string signature = msg.get(2);
                std::string service_name = msg.get(3);
                std::string payload = msg.get(4);
                std::map<std::string, Service*>::iterator it = services_.find(service_name);
                if (it == services_.end()){
                    // create a new service
                    Service * service = new Service(service_name);
                    service->AddWork(payload);
                    services_.insert(std::make_pair(service_name, service));
                    std::cout<<"Created a new service "<<service_name<<std::endl;
                } else{
//                    it->

                }
                std::cout<<"Service "<<service_name<<" has "<< services_[service_name]->TotalWork()<<" Items"<<std::endl;

            }


        }
    }
}
