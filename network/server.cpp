#include "server.h"
#include <iostream>

Server::Server(std::string ip, int port){
    // prepares the socket descriptor and the relevant structs
    ip_addr = ip;
    port = port;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        perror("Could not create server socket");
        exit(1);
    }

    //this will avoid the socket already in use if server is restarted and the socket 
    // is still lying around in te kernel

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt)) != 0){
        perror("Could not reuse socket");
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_addr.c_str(), &(server_addr.sin_addr)) <= 0){
        perror("Could not translate IP ");
        exit(1);
    } // lets work with IPV4 for now

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0 ){
        perror("Could not bind to socket");
        exit(1);
    }
}

void Server::start(){
    // binds the socket and then listens for incoming connections from clients
    // this is the beginning of the client server exchange


    if(listen(server_fd, LISTEN_QUEUE) !=0 ){
        perror("Server could not listen for connections");
        close(server_fd);
        exit(1);
    }
    fd_set fd_tracker; // tracks the file descriptors list
    fd_set fd_transact; // used in select /its temp
    int fd_max = server_fd;
    FD_ZERO(&fd_tracker);
    FD_ZERO(&fd_transact);
    FD_SET(server_fd, &fd_tracker); 
    struct sockaddr_in client_addr;
    char client_ip[INET6_ADDRSTRLEN];    
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
                    // accept new client connection
                    std::cout<<"Got a connection"<<std::endl;
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
                        // std::cout<<"New connection from: "<<inet_ntop(client_addr.sin_family,
                        //             &client_addr.sin_addr, client_ip, INET6_ADDRSTRLEN, 
                        //             client_fd);
                    }
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