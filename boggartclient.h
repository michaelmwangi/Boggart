#ifndef BOGGART_CLIENT_H
#define BOGGART_CLIENT_H

struct BoggartClient{
    int64_t id; // unique client Id assigned incrementally
    int file_descriptor; // client file descriptor socket
    std::string current_payload; // current payload from the client
    std::string current_command; // most recent command issued by the client
    std::string reply; // latest reply to the client
    std::string service; // refers to the service this client has subscribed to
};

#endif //BOGGART_CLIENT_H
