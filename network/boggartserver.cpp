#include "boggartserver.h"

Network::Network(std::string host, std::string port): host_(host), port_(port)
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

void Network::run(){
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

    while(true){
        read_fds = tracker;
        std::cout<<"waiting..."<<std::endl;
        // for now using select but will change to epoll later
        if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
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
                if(i == server_fd_){
                    // we have a new connection
                    // although we refer to the connection as a client. The connection request may also be from a worker
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
                        std::cout<<"Got a new connection"<<std::endl;
                        AddConnection(client_fd);
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
    }
}


void Network::ProcessIncomingData(const char * data, int fd){
    rapidjson::Document document;
    document.Parse(data);
    if (document.HasMember("signature") && document.HasMember("service") && document.HasMember("payload") && document.HasMember("command")){
        std::string signature = document["signature"].GetString();
        std::string service = document["service"].GetString();
        std::string payload = document["payload"].GetString();
        std::string command = document["command"].GetString();

        if (signature == OpDefinitions::client_signature){
            ProcessClientData(payload, service, fd);
        }else if(signature == OpDefinitions::worker_signature){
           ProcessWorkerData(payload);
        }else if(signature == OpDefinitions::internal_worker_signature){
//            ProcessInternalData(payload);
        }else{
            std::cout<<"Unknown signature passed... ignoring request"<<std::endl;
        }
    }else{
        std::cout<<"Not all mandatory fields [signature service payload] set"<<std::endl;
    }
       
}

void Network::ProcessClientData(std::string payload, std::string service, int fd){
 // fetch the client from store
    try{
        BoggartClient boggart_client = boggart_clients_.at(fd);
        boggart_client.current_payload = payload;

    } catch (const std::out_of_range &excp){
        std::cout<<"Could not get client for "<<fd<<std::endl;
    }

}

void Network::ProcessWorkerData(std::string payload, service, command, fd){
    if(command == OpCommands::worker_ready){
        // add new worker
    }else if(command == OpCommands::worker_resp){
        // get worker response
    }
}

void Network::AddConnection(int filedescriptor){
    BoggartClient boggart_client;
    boggart_client.file_descriptor = filedescriptor;
    boggart_client.current_payload = std::string("", READ_BUF_SIZE);
    boggart_client.id = 0; // TODO change this

    boggart_clients_[filedescriptor] = boggart_client;
}


void Network::RemoveConnection(int filedescriptor){
    boggart_clients_.erase(filedescriptor);
}
