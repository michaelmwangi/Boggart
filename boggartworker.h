#ifndef BOGGARTWORKER_H
#define BOGGARTWORKER_H

#include <chrono>

struct BoggartWorker{
    int64_t id; // unique worker id
    int file_descriptor; // socket file descriptor
    std::string current_command; // most recent issued command by this worker
    std::string current_response;
    int64_t last_activity; // last time in ms we saw action from this guy
};

#endif // BOGGARTWORKER_H
