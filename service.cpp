#include "service.h"


Service::Service(std::string servicename):
    servicename_ {servicename}
{

    // TODO find a way to keep track of this worker thread

    // each service has a single service worker thread that is used to
    // consume jobs from the service's job queue and push that to the ready worker
    std::thread t(std::bind(&Service::ConsumeJob, this));
    t.detach();
}

/**
 * @brief Service::ConsumeJob
 * The service worker thread uses this function to consume jobs from the queue and ship them
 * to the workers
 */
void Service::ConsumeJob(){
    std::cout<<"Started to consume on "<<servicename_<<std::endl;
    while(true){
        // grab worker first before we start consuming
        std::shared_ptr<BoggartWorker> worker = workers_.WaitAndPop();
        std::pair<std::string, std::string> work_payload = jobs_.WaitAndPop();
        std::string job_id = work_payload.first;
        std::string job = work_payload.second;
        // pass the job to the worker
        std::cout<<"consuming job"<<std::endl;
    }
}

/**
 * @brief Service::AddJob
 * adds the job payload to the service queue and also generates a unique id for the job. This ID is what
 * is used to identify the job
 * @param workpayload
 */
void Service::AddJob(std::string workpayload){
    uuid_t uid_out;
    uuid_generate_time(uid_out);
    char job_id[37];
    uuid_unparse_lower(uid_out, job_id);
    auto work = std::make_pair(job_id, workpayload);
    jobs_.Push(work);
}


bool Service::WorkerExists(int fd){
    auto pos = registered_workers_.find(fd);
    if (pos == registered_workers_.end()){
        return false;
    }
     return true;
}

/**
 * @brief Service::RegisterWorker
 * Adds workers to this service worker registry aftre they issue a ready command
 * @param workerfd: the Worker socket file descriptor
 */
void Service::RegisterWorker(std::shared_ptr<BoggartWorker> worker){
    registered_workers_[worker->file_descriptor] = worker;
}

/**
 * @brief Service::PurgeWorkers
 * Checks each worker last activity and proceeds to purge ones that havent whose lastactivity is beyond the
 * timeout threshhold for a worker set in the BoggartConfig
 */
void Service::PurgeWorkers(){
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    for(auto & workerentry: registered_workers_){
        auto worker = workerentry.second;
        std::chrono::duration<double> seconds_diff = now - worker->last_activity;
        std::cout<<"The last activity from this worker was "<<seconds_diff.count()<<std::endl;
    }
}

/**
 * @brief Service::AddWorker
 * adds a worker instance to the working pool from which work is assigned
 * @param worker
 */
void Service::AddWorker(std::shared_ptr<BoggartWorker> worker){
    workers_.Push(worker);
}

int Service::TotalJobs(){
    return jobs_.Size();
}
