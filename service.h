#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <thread>

struct Worker{
    std::string address; // address to use when routing messages
    int expiry; // time to expire before heartbeat

    Worker(std::string addr, int expire=0){
        address = addr;
        expiry = expire;
    }
};

class Service{
private:
    std::string servicename_;
    std::queue<std::string> requests_;
    std::queue<std::shared_ptr<Worker>> workers_;

public:
    Service(std::string);
    void AddWork(std::string);
    void AddWorker(std::shared_ptr<Worker>);
    int TotalWork();
};



#endif // SERVICE_H
