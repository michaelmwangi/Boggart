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

}