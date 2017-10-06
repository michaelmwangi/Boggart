#include "server.h"
#include <iostream>

Server::Server(std::string ip, int port){
    // prepares the socket descriptor and the relevant structs
    ip_addr = ip;
    port = port;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1){
        perror("Could not create server socket");
        exit(1);
    }
    int opt = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt)) != 0){
        perror("Could not reuse socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_addr, &(server_addr.sin_addr)) <= 0){
        perror("Could not translate IP "+ip_addr);
        exit(1);
    } // lets work with IPV4 for now

}

void Server::listen(){
    // binds the socket and then listens for incoming connections from clients
    // this is the beginning of the client server exchange

    //this will avoid the socket already in use if server is restarted and the socket 
    // is still lying around in te kernel

    if(bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0 ){
        perror("Could not bind to socket");
        exit(1);
    }

    if(listen(sock_fd, LISTEN_QUEUE) !=0 ){
        perror("Server could not listen for connections");
        close(sock_fd);
        exit(1);
    }
    fd_set fd_tracker; // tracks the file descriptors list
    fd_set fd_transact; // used in select /its temp
    int fd_max = sock_fd;
    FD_SET(sock_fd, fd_traker); 

    while(true){
        fd_transact = fd_tracker;

        if (select(fd_max+1, &fd_transact, NULL, NULL, NULL) == -1){
            perror("Error while select");
            exit(2);
        }
    }

}