#include "service.h"


Service::Service(std::unique_ptr<ServiceConfig> &serviceconfig): kJobIDLenght(20)
{
    service_config_ = std::move(serviceconfig);
    job_counter_ = 0;
    // TODO find a way to keep track of this worker thread

    // each service has a single service worker thread that is used to
    // consume jobs from the service's job queue and push that to the ready worker
    std::thread t(std::bind(&Service::ConsumeJob, this));
    t.detach();
}

/**
 * @brief Service::ConsumeJob
 * The service worker thread uses this function to consume jobs from the queue and ship them
 * to the workers. This worker is also charged with checking teh last activity of the selected worker against the worker_heart_beat_timeout
 * then makes a decision of purging the worker or not.
 */
void Service::ConsumeJob(){
    std::cout<<"Started to consume on "<<service_config_->name<<std::endl;
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
    std::string job_seed = GenerateUniqueID();
    std::string key = std::to_string(job_counter_);
    unsigned char * data = (unsigned char *) job_seed;
    uint res_len = 32;
    unsigned char *result;

    result = HMAC(EVP_sha256(), key.c_str(), key.length(), data,  kJobIDLenght, NULL, NULL);
    std::cout<<"The job id is "<<result<<std::endl;

    auto work = std::make_pair(job_id, workpayload);
    jobs_.Push(work);
}

std::string Service::GenerateUniqueID(){
    std::ifstream file_handle;
    file_handle.open("/dev/urandom", std::ifstream::in);
    int buffer_size = 20;
    char* seed  = new char[buffer_size];
    if(file_handle.is_open()){
        file_handle.read(seed, buffer_size);
        if(file_handle){
            std::string res(seed);
            delete seed;
            return std::move(res);
        }
    }

    std::cout<<"Could not generate the seed from /dev/urandom"<<std::endl;
    // TODO find a way to attempt to generate a trurly random and unique seed
    return "JUSTATEMPFX";
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
    auto worker_iter = registered_workers_.begin();
    while(worker_iter != registered_workers_.end()){
        auto worker = worker_iter->second;
        std::chrono::duration<double> seconds_diff = now - worker->last_activity;
        if (seconds_diff.count() >= service_config_->worker_heart_beat){
            std::cout<<"Removing worker "<<worker->id<<" last activity was "<< seconds_diff.count()<<" against a heart beat timeout of "<<service_config_->worker_heart_beat<<std::endl;
            auto mark = worker_iter;
            worker_iter++;
            registered_workers_.erase(mark);
            // TODO remove worker from worker pool
            // a bug lies here we need a way to prevent the backhround worker from pushing jobs to dead workers.i.e workers that have been marked for purging but are in the process of
            // being removed. We can have a global stop the service function while we are puging
            continue;
        }
        std::cout<<"The last activity from this worker was "<<seconds_diff.count()<<std::endl;
        worker_iter++;
    }
//    for(auto & workerentry: registered_workers_){
//        auto worker = workerentry.second;

//    }
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
