#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <deque>
#include <vector>

class Service{
private:
    std::string servicename_;
    std::vector<std::string> requests_;
    std::deque<std::string> worker_;

public:
    Service(std::string);
    void AddWork(std::string);
    int TotalWork();
};

#endif // SERVICE_H
