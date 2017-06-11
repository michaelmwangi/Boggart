#ifndef BOGGARTWORKER_H
#define BOGGARTWORKER_H

struct BoggartWorker{
    int64_t id;
    int file_descriptor;
    std::string current_command;
    std::string current_response;
};

#endif // BOGGARTWORKER_H
