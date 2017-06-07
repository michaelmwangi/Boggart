#ifndef BOGGART_CLIENT_H
#define BOGGART_CLIENT_H

struct BoggartClient{
    int64_t id; // unique client Id assigned incrementally
    int file_descriptor; // client file descriptor socket
    std::string current_payload; // current payload 
    std::string reply; // latest reply to the client
};

#endif //BOGGART_CLIENT_H