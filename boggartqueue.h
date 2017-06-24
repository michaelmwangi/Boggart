#ifndef REQUESTSQUEUE_H
#define REQUESTSQUEUE_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template<typename T>
class BoggartQueue{

private:
    std::mutex mut_;
    std::queue<T> queue_;
    std::condition_variable mcond_;
public:

    void Push(T payload){
        std::unique_lock<std::mutex> lock{mut_};
        queue_.push(payload);
        mcond_.notify_one();
    }

    T WaitAndPop(){
        std::unique_lock<std::mutex> lock{mut_};
        mcond_.wait(lock, [this]{return !queue_.empty();});
        T payload = queue_.front();
        queue_.pop();
        lock.unlock();
        return payload;
    }

    int Size(){
        return queue_.size();
    }

};

#endif // REQUESTSQUEUE_H
