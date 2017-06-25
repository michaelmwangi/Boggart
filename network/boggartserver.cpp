#include "boggartserver.h"

BoggartServer::BoggartServer(std::string host, std::string port): host_(host), port_(port)
{
    struct addrinfo hints, *info_list;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // ipv4, ipv6
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    int addr_res = getaddrinfo(host_.c_str(), port_.c_str(), &hints, &info_list);
    if (addr_res != 0){
        std::cout<<"Could not prepare the address info due to "<<gai_strerror(addr_res)<<std::endl;
        exit(1);
    }

    struct addrinfo *serv_info;
    for(serv_info=info_list; serv_info != NULL; info_list->ai_next){
        server_fd_ = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
        if (server_fd_ == -1){
            continue;
        }

        int bind_res = bind(server_fd_, serv_info->ai_addr, serv_info->ai_addrlen);
        if (bind_res == 0){
            break;
        }

        close(server_fd_);
    }
    if(serv_info == NULL){
        std::cout<<"Could not bind to "<<host_<<"::"<<port_<<std::endl;
        exit(1);
    }
    freeaddrinfo(info_list);
}

void BoggartServer::run(){
    if(listen(server_fd_, 255) == -1){
        std::cout<<"Could not listen "<<std::endl;
        perror("listen()");
    }

    fd_set tracker;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_ZERO(&tracker);
    FD_SET(server_fd_, &tracker);
    int fd_max = server_fd_;

    struct sockaddr_in client_addr;    
    char buf[READ_BUF_SIZE];
    socklen_t client_len;

    // time struct for worker heartbeat timeout
    timeval timeoutctl, timeout;
    timeoutctl.tv_sec = 3; // for now until we have a config file lets set the timeout value to 3 sec

    while(true){
        // select has an annoying behaviour of modifying the some structs passed in in a kind of valu<->result semantics
        timeout = timeoutctl;
        read_fds = tracker;
        bool timerexpired = true;
        std::cout<<"waiting..."<<std::endl;
        // for now using select but will change to epoll later
        if(select(fd_max+1, &read_fds, NULL, NULL, &timeout) == -1){
            std::cout<<"Could not wait for a connection"<<std::endl;

            if(errno == EINTR){
                continue;
            }else if(errno == EINVAL){
                std::cout<<fd_max+1<< " file descriptors cannot be supported"<<std::endl;
                exit(1);
            }else if(errno == ENOMEM){
                std::cout<<"Unable to allocate memory"<<std::endl;
                continue;
            }
        }

        for(int i=0; i <= fd_max; i++){
            if(FD_ISSET(i, &read_fds)){
                timerexpired = false;
                if(i == server_fd_){
                    // we have a new connection
                    // although we refer to the connection as a client. -> (BoggartWorker or BoggartClient)
                    client_len = sizeof(client_addr);

                    int client_fd = accept(server_fd_, (struct sockaddr *)&client_addr, &client_len);

                    if(client_fd == -1){
                        perror("accept()");
                        continue;
                    }else{
                        FD_SET(client_fd, &tracker);

                        if(client_fd > fd_max){
                            fd_max = client_fd;
                        }                        
                    }
                }else{
                    // someone just sent us some data
                    int num_read = recv(i, buf, READ_BUF_SIZE, 0);
                    if(num_read <= 0){
                        if(num_read == 0){
                            std::cout<<"Client closed connection"<<std::endl;
                        }else{
                            perror("recv()");
                        }

                        close(i);
                        FD_CLR(i, &tracker);
                    }else{
                        std::cout<<"Read these bytes "<<buf<<std::endl;

                        // plugin into boggart space now
                        ProcessIncomingData(buf, i);
                        memset(buf, 0, READ_BUF_SIZE);
                    }
                }
            }
        }

        // maybe the timer expired hmm
        if (timerexpired){
            for (auto & service : services_){
                service.second->PurgeWorkers();
            }
        }
    }
}

/**
 * @brief BoggartServer::ProcessIncomingData
 * This acts as the router function when data is received by boggart by examining the signatures
 * of the payload
 * @param data: the incoming payload from the client (boggart worker or boggart client)
 * @param fd: socket file descriptor
 */
void BoggartServer::ProcessIncomingData(const char * data, int fd){
    rapidjson::Document document;
    document.Parse(data);
    if (document.HasMember("signature") && document.HasMember("service") && document.HasMember("payload") && document.HasMember("command")){
        std::string signature = document["signature"].GetString();
        std::string service = document["service"].GetString();
        std::string payload = document["payload"].GetString();
        std::string command = document["command"].GetString();

        if (signature == Signatures::client_signature){
            std::cout<<"Processing data from the client"<<std::endl;
            ProcessClientData(payload, service, fd);
        }else if(signature == Signatures::worker_signature){
            std::cout<<"Processing data from the worker"<<std::endl;
           ProcessWorkerData(payload, service, command, fd);
        }else if(signature == Signatures::internal_worker_signature){
//            ProcessInternalData(payload);
        }else{
            std::cout<<"Unknown signature passed... ignoring request"<<std::endl;
        }
    }else{
        std::cout<<"Not all mandatory fields [signature service payload command] set"<<std::endl;
    }
       
}

void BoggartServer::ProcessClientData(std::string payload, std::string servicename, int fd){
 // fetch the client from store
    try{
        std::shared_ptr<Service> service = GetService(servicename);
        service->AddJob(payload);
        std::cout<<"Total jobs pending in service "<<servicename<<" "<<service->TotalJobs()<<std::endl;
    } catch (const std::out_of_range &excp){
        std::cout<<"Could not get client for "<<fd<<std::endl;
    }

}

/**
 * @brief BoggartServer::GetService
 * gets the service using the servicename param as a unique identifier. If the service is not found a new one is created
 * using the servicename identifier
 * @param servicename
 */
std::shared_ptr<Service> BoggartServer::GetService(std::string servicename){
    try{
         return services_.at(servicename);
    } catch (const std::out_of_range &excp){
        // we could not get that service,proceed to create a new one
        auto service = std::make_shared<Service>(servicename);
        services_[servicename] = service;
        std::cout<<"created a new service "<<servicename<<std::endl;
        return service;
    }

//    return service;
}

void BoggartServer::ProcessWorkerData(std::string payload, std::string servicename, std::string command, int fd){
    std::cout<<"Inside the worker "<<std::endl;
    if(command == OpCommands::worker_ready){        
        std::shared_ptr<Service> service = GetService(servicename);
        // check if we already have this worker already registered
        bool exists = service->WorkerExists(fd);
        if (exists){
            std::cout<<"Worker with filedescriptor "<<fd<<" tried to register twice.. ignoring for now"<<std::endl;
        }else{

            auto worker = std::make_shared<BoggartWorker>();
            worker->current_command = command;
            worker->file_descriptor = fd;
            worker->id = 0; // TODO change this to issue ID uniquely and incrementaly
            worker->last_activity = std::chrono::system_clock::now();

            service->RegisterWorker(worker);
            service->AddWorker(worker);

        }
    }else if(command == OpCommands::worker_resp){
        // get worker response
    }
}

