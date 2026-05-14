
#include "utils/Queue.h"

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>


// ---------- Basic operations ----------
TEST(BoundedBlockingQueueTest, PushPopSingle) {
    BoundedBlockingQueue<int> q(10);
    q.push(42);
    int val = 0;
    ASSERT_TRUE(q.pop(val));
    EXPECT_EQ(val, 42);
}

TEST(BoundedBlockingQueueTest, TryPushTryPop) {
    BoundedBlockingQueue<int> q(2);
    EXPECT_TRUE(q.try_push(1));
    EXPECT_TRUE(q.try_push(2));
    EXPECT_FALSE(q.try_push(3));

    int v = 0;
    EXPECT_TRUE(q.try_pop(v));
    EXPECT_EQ(v, 1);
    EXPECT_TRUE(q.try_pop(v));
    EXPECT_EQ(v, 2);
    EXPECT_FALSE(q.try_pop(v));
}

// ---------- Blocking behavior ----------
TEST(BoundedBlockingQueueTest, PushBlocksWhenFull) {
    BoundedBlockingQueue<int> q(1);
    q.push(100);

    std::atomic<bool> push_done{false};
    std::thread pusher([&]() {
        q.push(200);
        push_done = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(push_done);

    int v = 0;
    ASSERT_TRUE(q.pop(v));
    EXPECT_EQ(v, 100);

    pusher.join();
    EXPECT_TRUE(push_done);

    ASSERT_TRUE(q.pop(v));
    EXPECT_EQ(v, 200);
}

TEST(BoundedBlockingQueueTest, PopBlocksWhenEmpty) {
    BoundedBlockingQueue<int> q(1);
    std::atomic<bool> pop_done{false};
    int popped = 0;

    std::thread popper([&]() {
        int v;
        bool ok = q.pop(v);
        popped = v;
        pop_done = ok;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(pop_done);

    q.push(99);
    popper.join();

    EXPECT_TRUE(pop_done);
    EXPECT_EQ(popped, 99);
}

// ---------- Closing ----------
TEST(BoundedBlockingQueueTest, CloseUnblocksPushWithException) {
    BoundedBlockingQueue<int> q(0);
    std::atomic<bool> push_throw{false};
    std::thread pusher([&]() {
        try {
            q.push(5);
        } catch (const std::runtime_error&) {
            push_throw = true;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(push_throw);

    q.close();
    pusher.join();
    EXPECT_TRUE(push_throw);
}

TEST(BoundedBlockingQueueTest, CloseUnblocksPop) {
    BoundedBlockingQueue<int> q(1);
    std::atomic<bool> pop_returned{false};
    std::thread popper([&]() {
        int v;
        bool ok = q.pop(v);
        pop_returned = ok;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(pop_returned);

    q.close();
    popper.join();
    EXPECT_FALSE(pop_returned);
}

TEST(BoundedBlockingQueueTest, OperationsAfterClose) {
    BoundedBlockingQueue<int> q(5);
    q.push(7);
    q.close();

    int v = 0;
    EXPECT_TRUE(q.pop(v));
    EXPECT_EQ(v, 7);

    EXPECT_FALSE(q.pop(v));
    EXPECT_FALSE(q.try_pop(v));
    EXPECT_THROW(q.push(1), std::runtime_error);
    EXPECT_FALSE(q.try_push(2));
}

// ---------- Concurrency ----------
TEST(BoundedBlockingQueueTest, MultipleProducersConsumers) {
    constexpr int kProducers = 4;
    constexpr int kConsumers = 4;
    constexpr int kItemsPer = 2500;
    constexpr size_t kCap = 16;

    BoundedBlockingQueue<int> q(kCap);
    std::atomic<int> sum_produced{0};
    std::atomic<int> sum_consumed{0};
    std::atomic<int> items_consumed{0};

    auto producer = [&](int start) {
        for (int i = 0; i < kItemsPer; ++i) {
            q.push(start + i);
            sum_produced += (start + i);
        }
    };

    auto consumer = [&]() {
        while (true) {
            int v;
            if (!q.pop(v)) break;
            sum_consumed += v;
            ++items_consumed;
        }
    };

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    for (int i = 0; i < kProducers; ++i)
        producers.emplace_back(producer, i * kItemsPer);
    for (int i = 0; i < kConsumers; ++i)
        consumers.emplace_back(consumer);

    for (auto& t : producers) t.join();
    q.close();
    for (auto& t : consumers) t.join();

    EXPECT_EQ(items_consumed, kProducers * kItemsPer);
    EXPECT_EQ(sum_produced, sum_consumed);
}

// ---------- Move-only types ----------
TEST(BoundedBlockingQueueTest, MoveOnlyElements) {
    BoundedBlockingQueue<std::unique_ptr<int>> q(2);
    q.push(std::make_unique<int>(10));
    q.push(std::make_unique<int>(20));

    std::unique_ptr<int> p1, p2;
    ASSERT_TRUE(q.try_pop(p1));
    EXPECT_EQ(*p1, 10);
    ASSERT_TRUE(q.pop(p2));
    EXPECT_EQ(*p2, 20);
}