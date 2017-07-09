#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <uuid/uuid.h>
#include <openssl/hmac.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include "boggartqueue.h"
#include "boggartworker.h"
#include "boggartconfig.h"

struct ServiceConfig{
    std::string name;
    int worker_heart_beat;
};

class Service{    
private:

    std::unique_ptr<ServiceConfig> service_config_;
    std::string servicename_;
    const int kJobIDLenght;
    int64_t job_counter_;
    std::shared_ptr<BoggartConfig> boggart_config_; // we should
    std::unordered_map<int, std::shared_ptr<BoggartWorker>> registered_workers_; // holds workers who have issued a READY cmd and are still active
    BoggartQueue<std::shared_ptr<BoggartWorker>> workers_; // Holds workers that are active and are being shuffled in between jobs
    BoggartQueue<std::pair<std::string, std::string>> jobs_; // holds the jobs to be processed by workers
    BoggartQueue<std::pair<std::string, std::string>> async_job_results_; // holds the result of asynchronous job results
    BoggartQueue<std::pair<std::string, std::string>> sync_job_results_; // // holds the result of synchronous job results
    std::string GenerateUniqueID();
public:
    Service(std::unique_ptr<ServiceConfig> &serviceconfig);
    std::string GetResult(std::string jobid, bool sync=false);
    void AddJob(std::string workpayload);
    void RegisterWorker(std::shared_ptr<BoggartWorker> worker);
    void PurgeWorkers();
    void AddWorker(std::shared_ptr<BoggartWorker> worker);
    bool WorkerExists(int fd);
    int TotalJobs();
    void ConsumeJob();    
};



#endif // SERVICE_H
