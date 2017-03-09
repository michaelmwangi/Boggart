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

    server_->bind(endpoint_url_);
    poller.add(*server_.get());

    while(true){
        bool has_items = poller.poll();
        if (has_items){
            zmqpp::message msg;
            server_->receive(msg);            

            std::string signature = msg.get(2);
            if(signature == "BOGC01"){
                // handle client request
                if(msg.parts() >= 5){
                    HandleClient(msg.copy());
                } else{
                    zmqpp::message ret_msg {msg.get(0), "", "Unsatisfiable number of arguments passed"};
                    server_->send(ret_msg);
                }
           }else if(signature == "BOGW01"){
                // handle worker requests
                std::cout<<"Handling worker request "<< msg.parts()<<std::endl;
                std::string service_name = msg.get(4);
                HandleWorker(msg.copy());
           }else if(signature == "BOGI01"){
                HandleInternalCommunication(msg.copy());
           }else{
                zmqpp::message ret_msg {msg.get(0), "", "Unknown signature passed "+msg.get(2)};
                server_->send(ret_msg);
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

void Broker::HandleClient(zmqpp::message msg){
    std::string client_addr = msg.get(0);
    std::string client_cmd = msg.get(3);
    if (client_cmd == "INSJOB"){
        std::string service_name = msg.get(4);
        std::string payload = msg.get(5);
        std::string blocking = msg.get(6);
        uuid_t uid_out;
        uuid_generate_time(uid_out);
        char job_id[37];
        uuid_unparse_lower(uid_out, job_id);
        if (blocking == "True"){
            sync_work_results_.insert(std::make_pair(job_id, client_addr));
        }else{
            async_work_results_.insert(std::make_pair(job_id, ""));
            // send back the job id
            // for async jobs the client uses a Dealer socket hence we just pump in the zmqp message frames
            zmqpp::message ret_msg {client_addr, job_id};
            std::cout<<"sending back job id for async job"<<std::endl;
            server_->send(ret_msg);

        }

        std::shared_ptr<Service> service = GetService(service_name);
        service->AddRequest(job_id, payload);
        std::cout<<"Service "<<service_name<<" has "<< services_[service_name]->TotalRequests()<<" Items"<<std::endl;
    }else if (client_cmd == "GETJOB"){
        std::cout<<"Getting the job"<<std::endl;
        std::string job_id = msg.get(4);
        std::string job_results;
        OpCodes ret_code = GetJobResults(job_id, job_results);
        int int_ret_code = static_cast<std::underlying_type<OpCodes>::type>(ret_code);
        zmqpp::message msg;
        msg.push_back(client_addr);
        msg.push_back("");
        msg.push_back(std::to_string(int_ret_code));
        msg.push_back(job_results);
        server_->send(msg);
    }

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
        std::string resp = msg.get(6);
        std::string job_id = msg.get(5);
        // if the resp is for a synchronous payload notify client immediately

        std::cout<<"Worker resp "<<resp<<std::endl;
        auto exists = sync_work_results_.find(job_id);
        if (exists != sync_work_results_.end()){
            // sync_work_results structure job-id -> client_addr
            std::string client_addr = exists->second;
            zmqpp::message msg;
            msg.push_back(client_addr);
            msg.push_back("");
            msg.push_back(resp);
            std::cout<<"Sending results back to the client"<<client_addr<<std::endl;
            server_->send(msg);
            std::cout<<"Finished sending results back to client"<<std::endl;
            sync_work_results_.erase(job_id);
        }else{
            // store async work
            async_work_results_[job_id] =  resp;
        }


        service->AddWorker(worker);
    }

}

void Broker::HandleInternalCommunication(zmqpp::message msg){
    std::string type = msg.get(3);
    std::cout<<"Type is "<<type<<std::endl;
    if(type == "WORK"){
        // we are pushing work to the worker
        std::string worker_addr = msg.get(4);
        std::string job_id = msg.get(5);
        std::string work_payload = msg.get(6);
        zmqpp::message work;
        work.push_back(worker_addr);
        work.push_back(job_id);
        work.push_back(work_payload);
        std::cout<<"Sending work to worker "<<worker_addr<<std::endl;
        server_->send(work);
    }
}

OpCodes Broker::GetJobResults(std::string jobid, std::string& job_res){
    auto job = async_work_results_.find(jobid);
    OpCodes resp_code;
    if (job != async_work_results_.end()){
        job_res = job->second;
        if (job_res == ""){
            resp_code = OpCodes::pending;
            job_res = "Pending";
        }else{
            resp_code = OpCodes::ok;
        }

    }else{
        job_res = jobid + " does not exist";
        resp_code = OpCodes::not_found;
    }

    return resp_code;
}
