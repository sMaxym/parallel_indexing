#include "./../header/concurqueue.h"

template class concur_queue<std::string>;
template class concur_queue<std::map<std::string, size_t>>;

template<class T>
concur_queue<T>::concur_queue() { }

template<class T>
void concur_queue<T>::push(T d) {
    {
        std::lock_guard<std::mutex> lg{mutex_};
        queue_.push_back(d);
    }
    cv_.notify_one();
}

template<class T>
T concur_queue<T>::pop() {
    std::unique_lock<std::mutex> lg{mutex_};
    cv_.wait(lg, [this](){return queue_.size() != 0;});
    T d = queue_.front();
    queue_.pop_front();
    return d;
}

template<class T>
std::pair<T, T> concur_queue<T>::pop2() {
    std::unique_lock<std::mutex> lg{mutex_};
    cv_.wait(lg, [this](){return queue_.size() > 1;});
    T d1 = queue_.front();
    queue_.pop_front();
    T d2 = queue_.front();
    queue_.pop_front();
    return std::pair<T, T>(d1, d2);
}

template<class T>
size_t concur_queue<T>::get_size() {
    std::lock_guard<std::mutex> lg{mutex_};
    return queue_.size();
}
