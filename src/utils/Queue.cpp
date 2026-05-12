#include "utils/Queue.h"

template<typename T>
void BoundedBlockingQueue<T>::close()  {
    {
        std::lock_guard<std::mutex> lk(mutex);
        closed = true;
    }
    not_empty.notify_all();
    not_full.notify_all();
}

template<typename T>
void BoundedBlockingQueue<T>::push(T &&item) {
    std::unique_lock<std::mutex> lk(mutex);
    not_full.wait(lk, [this]() {
        return content.size() < capacity || closed;
    });
    if (closed) {
        throw std::runtime_error("push into closed queue");
    }
    content.push_back(std::move(item));
    lk.unlock();
    not_empty.notify_one();
}

template<typename T>
bool BoundedBlockingQueue<T>::try_push(T &&item)  {
    std::scoped_lock<std::mutex> lk(mutex);
    if (content.size() == capacity || closed) {
        return false;
    }
    content.push_back(std::move(item));
    not_empty.notify_one();
    return true;
}

template<typename T>
bool BoundedBlockingQueue<T>::pop(T &item){
    std::unique_lock<std::mutex> lk(mutex);
    not_empty.wait(lk, [this]() {
        return !content.empty() || closed;
    });
    if (content.empty()) {
        return false;
    }
    item = std::move(content.front());
    content.pop_front();
    lk.unlock();
    not_full.notify_one();
    return true;
}

template<typename T>
bool BoundedBlockingQueue<T>::try_pop(T &item)  {
    std::lock_guard<std::mutex> lk(mutex);
    if (content.empty()) {
        return false;
    }
    item = std::move(content.front());
    content.pop_front();
    not_full.notify_one();
    return true;
}
