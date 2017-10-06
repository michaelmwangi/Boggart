#include "netdefinitions.h"
#include <string>

class Server{
private:
    struct sockaddr_in server_addr;
    std::string ip_addr;
    int port;
    int sock_fd;
public:
    Server(std::string ip, int port);
    void listen();
}