#ifndef EVENT_LOOP
#include <map>

struct Client{
    int id; // unique id of the client
    std::string ip_addr; // this client's ip address
    std::string recv_buf; //holds the data just received from client not yet processed
    std::string send_buf; // holds data yet to be sent to the client
    std::string cmd; // the current command issued by the client 
};

class EventLoop{
private:
    int server_fd; // the server socker file descriptor
    int max_fd;
    std::unordered_map<int, Client> clients;
    void new_client();
    void remove_client(int clientfd);
    bool client_exists(int clientfd);
    void recv_from_client(int clientfd);
    void send_to_client(int clientfd);
public:
    EventLoop(int serv_fd);
    void loop();
};
#define EVENT_LOOP