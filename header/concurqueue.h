#ifndef CONCURQUEUE_H
#define CONCURQUEUE_H

#include <map>
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>

template<class T>
class concur_queue {
private:
    std::deque<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
public:
    concur_queue();

    void push(T d);
    T pop();
    size_t get_size();
};

#endif // CONCURQUEUE_H
