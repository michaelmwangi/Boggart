#ifndef EVENT_LOOP

struct 

class EventLoop{
    int server_fd; // the server socker file descriptor
    void new_client();
    void recv_from_client();
    void send_to_client();
public:
    EventLoop(int serv_fd);
    void loop();
};
#define EVENT_LOOP