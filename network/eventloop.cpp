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
        FD_SET(client_fd, &fd_tracker); // start tracking the client fd
        if(client_fd > fd_max){
            fd_max = client_fd;
        }
        char hostname_buf[NI_MAXHOST];
        char port_buf[NI_MAXSERV];
        if(getnameinfo((struct sockaddr*)&client_addr, client_len, hostname_buf, NI_MAXHOST, port_buf, NI_MAXSERV, 0) == 0){
            std::cout<<"Got a new connection from "<<hostname_buf<<":"<<port_buf<<std::endl;
        }else{
            perror("Error getting the client IP info");
        }
    }
}

/**
 * reads data from the client and adds to recv buffer of the client
 * */
void EventLoop::recv_from_client(){

}

/**
 * Kick starts the main event loop and listens for new connections
 * */
void EventLoop::loop(){    
    fd_set fd_tracker; // tracks the file descriptors list
    fd_set fd_transact; // used in select /its temp
    int fd_max = server_fd;
    FD_ZERO(&fd_tracker);
    FD_ZERO(&fd_transact);
    FD_SET(server_fd, &fd_tracker); 
        
    char buf[BUF_SIZE];
    int client_fd;
    while(true){
        fd_transact = fd_tracker;
        std::cout<<"waiting for connections"<<std::endl;
        if (select(fd_max+1, &fd_transact, NULL, NULL, NULL) == -1){
            perror("Error while select");
            exit(2);
        }
        
        for(int i=0;i <= fd_max;i++){
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