#ifndef BOGGARTWORKER_H
#define BOGGARTWORKER_H

struct BoggartWorker{
    int64_t id; // unique worker id
    int file_descriptor; // socket file descriptor
    std::string current_command; // most recent issued command by this worker
    std::string current_response;
    // TODO include lastactivity attr
};

#endif // BOGGARTWORKER_H
