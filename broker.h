#ifndef BROKER_H
#define BROKER_H

#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <uuid/uuid.h>
#include <zmqpp/zmqpp.hpp>
#include <glog/logging.h>
#include "service.h"
#include "opcodes.h"
#include "opdefinitions.h"


class Broker{
private:
    std::string host_;
    std::string port_;
    std::string endpoint_url_;    
    OpDefinitions opdefinitions_;
    std::unique_ptr<zmqpp::socket> server_;
    std::map<std::string, std::shared_ptr<Service>> services_;
    std::map<std::string, std::string> async_work_results_;
    std::map<std::string, std::string> sync_work_results_;
    std::shared_ptr<Service> GetService(std::string service);
    OpCodes GetJobResults(std::string jobid, std::string& job_res);
    void HandleClient(zmqpp::message msg);
    void HandleWorker(zmqpp::message msg);
    void HandleInternalCommunication(zmqpp::message msg);
public:
    Broker(std::string qhost, std::string qport);
//    ~Broker();
    void StartBroker();
};

#endif // BROKER_H
