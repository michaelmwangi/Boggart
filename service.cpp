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

void Service::AddWorker(std::shared_ptr<BoggartWorker> worker){
    workers_.Push(worker);
}

int Service::TotalJobs(){
    return jobs_.Size();
}
