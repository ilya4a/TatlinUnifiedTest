#ifndef TATLINUNIFIEDTEST_QUEUE_H
#define TATLINUNIFIEDTEST_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template<typename T>
class BoundedBlockingQueue {
    std::deque<T> content;

    size_t capacity;

    std::mutex mutex;
    std::condition_variable not_empty;
    std::condition_variable not_full;

    bool closed = false;

    BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
    BoundedBlockingQueue(BoundedBlockingQueue &&) = delete;
    BoundedBlockingQueue &operator=(const BoundedBlockingQueue &) = delete;
    BoundedBlockingQueue &operator=(BoundedBlockingQueue &&) = delete;

public:
    explicit BoundedBlockingQueue(size_t capacity) : capacity(capacity) {}

    void close();

    void push(T&& item);

    bool try_push(T&& item);

    bool pop(T& item);

    bool try_pop(T& item);
};

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



#endif // TATLINUNIFIEDTEST_QUEUE_H
