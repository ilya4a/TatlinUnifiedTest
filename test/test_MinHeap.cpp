#include <algorithm>
#include <gtest/gtest.h>

#include <functional>
#include <stdexcept>
#include <vector>

#include "utils/MinHeap.h"

// ---------- Fixture for int min-heap ----------

class MinHeapTest : public ::testing::Test {
  protected:
    std::function<bool(int, int)> comp_;

    void SetUp() override {
        comp_ = [](int a, int b) { return a < b; };
    }
};

// ---------- Basic properties ----------

TEST_F(MinHeapTest, EmptyHeap) {
    MinHeap<int> heap(10, comp_);
    EXPECT_TRUE(heap.empty());
    EXPECT_THROW(heap.peek(), std::runtime_error);
    EXPECT_THROW(heap.extractMin(), std::runtime_error);
}

TEST_F(MinHeapTest, SingleElement) {
    MinHeap<int> heap(10, comp_);
    heap.insertNode(42);
    EXPECT_FALSE(heap.empty());
    EXPECT_EQ(heap.peek(), 42);
    EXPECT_EQ(heap.extractMin(), 42);
    EXPECT_TRUE(heap.empty());
}

// ---------- Insert & extract order ----------

TEST_F(MinHeapTest, InsertMultipleAndExtractAll) {
    MinHeap<int> heap(10, comp_);
    std::vector<int> input = { 5, 2, 8, 1, 9, 3 };
    for (int v : input) {
        heap.insertNode(v);
    }
    EXPECT_FALSE(heap.empty());

    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extractMin());
    }

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(extracted, expected);
}

TEST_F(MinHeapTest, DuplicateValues) {
    MinHeap<int> heap(10, comp_);
    std::vector<int> values = { 5, 1, 5, 1, 3 };
    for (int v : values) {
        heap.insertNode(v);
    }
    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extractMin());
    }
    std::vector<int> expected = { 1, 1, 3, 5, 5 };
    EXPECT_EQ(extracted, expected);
}

// ---------- Capacity growth ----------

TEST_F(MinHeapTest, CapacityExpandsWhenFull) {
    MinHeap<int> heap(2, comp_);
    for (int i = 0; i < 100; ++i) {
        heap.insertNode(100 - i);
    }
    EXPECT_FALSE(heap.empty());

    int prev = std::numeric_limits<int>::min();
    while (!heap.empty()) {
        int cur = heap.extractMin();
        EXPECT_LE(prev, cur);
        prev = cur;
    }
}

// ---------- Peek does not modify heap ----------

TEST_F(MinHeapTest, PeekDoesNotRemoveElement) {
    MinHeap<int> heap(10, comp_);
    heap.insertNode(7);
    heap.insertNode(2);
    EXPECT_EQ(heap.peek(), 2);
    EXPECT_EQ(heap.peek(), 2);
    EXPECT_EQ(heap.extractMin(), 2);
    EXPECT_EQ(heap.peek(), 7);
}

// ---------- Build heap from vector ----------

TEST_F(MinHeapTest, BuildHeapFromVector) {
    MinHeap<int> heap(0, comp_);
    std::vector<int> input = { 9, 3, 6, 1, 4, 8, 2, 7, 5 };
    heap.buildHeap(input);
    EXPECT_FALSE(heap.empty());

    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extractMin());
    }

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(extracted, expected);
}

// ---------- Delete node ----------

TEST_F(MinHeapTest, DeleteExistingNode) {
    MinHeap<int> heap(10, comp_);
    std::vector<int> data = { 10, 20, 30, 40, 50 };
    for (int v : data) {
        heap.insertNode(v);
    }
    EXPECT_FALSE(heap.empty());

    heap.DeleteNode(30);
    std::vector<int> remaining;
    while (!heap.empty()) {
        remaining.push_back(heap.extractMin());
    }
    std::vector<int> expected = { 10, 20, 40, 50 };
    EXPECT_EQ(remaining, expected);
}

TEST_F(MinHeapTest, DeleteNonExistentNode) {
    MinHeap<int> heap(10, comp_);
    heap.insertNode(1);
    heap.insertNode(2);
    size_t sizeBefore = 2;
    heap.DeleteNode(99);
    EXPECT_FALSE(heap.empty());

    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extractMin());
    }
    std::vector<int> expected = { 1, 2 };
    EXPECT_EQ(extracted, expected);
}

TEST_F(MinHeapTest, DeleteRoot) {
    MinHeap<int> heap(10, comp_);
    heap.insertNode(10);
    heap.insertNode(5);
    heap.insertNode(15);
    heap.DeleteNode(5);
    EXPECT_EQ(heap.peek(), 10);
    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extractMin());
    }
    std::vector<int> expected = { 10, 15 };
    EXPECT_EQ(extracted, expected);
}

// ---------- Custom comparator (max-heap) ----------

TEST(MinHeapCustomComp, MaxHeapBehaviour) {
    auto comp = [](int a, int b) { return a > b; }; // greater -> max-heap
    MinHeap<int> heap(10, comp);
    heap.insertNode(1);
    heap.insertNode(5);
    heap.insertNode(3);
    EXPECT_EQ(heap.peek(), 5);
    EXPECT_EQ(heap.extractMin(), 5);
    EXPECT_EQ(heap.peek(), 3);
    heap.insertNode(10);
    EXPECT_EQ(heap.extractMin(), 10);
    EXPECT_EQ(heap.extractMin(), 3);
    EXPECT_EQ(heap.extractMin(), 1);
}

// ---------- Pair type (like TapeSorter usage) ----------

TEST(MinHeapPairTest, PairMinByFirstElement) {
    auto comp = [](const std::pair<int32_t, size_t> &a, const std::pair<int32_t, size_t> &b) {
        return a.first < b.first;
    };
    MinHeap<std::pair<int32_t, size_t>> heap(10, comp);
    heap.insertNode({ 3, 0 });
    heap.insertNode({ 1, 1 });
    heap.insertNode({ 2, 2 });
    auto min = heap.extractMin();
    EXPECT_EQ(min.first, 1);
    EXPECT_EQ(min.second, 1);
    EXPECT_EQ(heap.peek().first, 2);
}
