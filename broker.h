#ifndef BROKER_H
#define BROKER_H

#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include "service.h"


class Broker{
private:
    std::string host_;
    std::string port_;
    std::string endpoint_url_;
    std::map<std::string, Service*> services_;
public:
    Broker(std::string qhost, std::string qport);
//    ~Broker();
    void StartBroker();
};

#endif // BROKER_H
