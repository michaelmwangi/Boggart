#include "eventloop.h"
#include "netdefinitions.h"


EventLoop::EventLoop(int serv_fd) : server_fd{serv_fd}
{
}

/**
 * Accepts new client connections and adds the client to the clients list 
 * */
void EventLoop::new_client(){
    struct sockaddr_in client_addr;
    char client_ip[INET6_ADDRSTRLEN];
    socklen_t client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr  *)&client_addr, &client_len);
    if(client_fd == -1){
        perror("Error accepting connection from client");
        FD_CLR(client_fd, &fd_tracker);
    }else{
        
        if (client_fd >= MAXFDS){
            std::cout<<"Cannot accept client MAXFD reached"<<std::endl;

        }else{
            FD_SET(client_fd, &fd_tracker); // start tracking the client fd
            if(client_fd > max_fd){
                max_fd = client_fd;
            }
            Client client = new Client();
            client->id = 123 ;// TODO Im changing this placeholder            
            char hostname_buf[NI_MAXHOST];
            char port_buf[NI_MAXSERV];
            if(getnameinfo((struct sockaddr*)&client_addr, client_len, hostname_buf, NI_MAXHOST, port_buf, NI_MAXSERV, 0) == 0){
                std::cout<<"Got a new connection from "<<hostname_buf<<":"<<port_buf<<std::endl;
                client->ip_addr = hostname_buf;
            }else{
                perror("Error getting the client IP info");
            }

            clients.insert({client_fd, client});
        }
        
    }
}

bool EventLoop::client_exists(int client_fd){
    auto found_iter = clients.find(client_fd);
    return found_iter == clients.end();
}

void EventLoop::remove_client(int client_fd){ 
    auto found_iter = clients.find(client_fd);   
    if(found_iter != clients.end()){
        clients.erase(found_iter);
    }
}

/**
 * reads data from the client connection and adds to recv buffer of the client
 * */
void EventLoop::recv_from_client(int clientfd){
    auto client_iter = clients.find(client_fd);
    if(client_iter != clients.end()){
        Client client = client_iter->second;
        if((int numread = read(clientfd, &client.recv_buf, BUF_SIZE)) <=0){
            if(numread == 0){
                // we have reached EOF probably close the client socket
                std::cout<<"Client closed connection"<<std::endl;
                remove_client(client_fd);
            }else{
                // be more specific here and also log data pointing out to which client 
                perror("Error reading from client")
            }
        }
    }
    
}

/**
 * Kick starts the main event loop and listens for new connections
 * */
void EventLoop::loop(){    
    fd_set fd_tracker; // tracks the file descriptors list
    fd_set fd_transact; // used in select /its temp
    max_fd = server_fd;
    FD_ZERO(&fd_tracker);
    FD_ZERO(&fd_transact);
    FD_SET(server_fd, &fd_tracker); 
        
    while(true){
        fd_transact = fd_tracker;
        std::cout<<"waiting for connections"<<std::endl;
        if (select(max_fd+1, &fd_transact, NULL, NULL, NULL) == -1){
            perror("Error while select");
            exit(2);
        }
        
        for(int i=0;i <= max_fd;i++){
            if(FD_ISSET(i, &fd_transact)){
                if(i == server_fd){
                    new_client();
                }else{
                    // read data from client
                    int num_bytes;
                    if((num_bytes = recv(i, buf, sizeof(buf), 0) <= 0)){
                        if(num_bytes == 0){
                            std::cout<<"Client closed connection"<<std::endl;
                            close(i);                            
                        }else{
                            perror("Error reading from client");
                        }
                        FD_CLR(client_fd, &fd_tracker);
                    }else{
                        std::cout<<"recieved "<<buf<<std::endl;
                    }
                }
            }
        }
    }
}