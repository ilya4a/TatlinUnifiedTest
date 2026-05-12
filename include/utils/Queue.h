#ifndef TATLINUNIFIEDTEST_QUEUE_H
#define TATLINUNIFIEDTEST_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

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


#endif // TATLINUNIFIEDTEST_QUEUE_H
