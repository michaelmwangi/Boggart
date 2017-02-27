#ifndef REQUESTSQUEUE_H
#define REQUESTSQUEUE_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

class RequestsQueue{
private:
    std::mutex mut_;
    std::queue<std::string> requests_;
    std::condition_variable mcond_;
public:

    void Push(std::string payload){
        std::unique_lock<std::mutex> lock{mut_};
        requests_.push(payload);
        mcond_.notify_one();
    }

    std::string WaitAndPop(){
        std::unique_lock<std::mutex> lock{mut_};
        mcond_.wait(lock, [this]{return !requests_.empty();});
        std::string payload = requests_.front();
        requests_.pop();
        lock.unlock();
        return payload;
    }

    int Size(){
        return requests_.size();
    }
};

#endif // REQUESTSQUEUE_H
