#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include "MockTape.h"
#include "sorting/TapeSorter.h"

class TapeSorterTest : public ::testing::Test {
  protected:
    std::function<std::unique_ptr<TapeI>()> factory_;

    void SetUp() override {
        factory_ = []() { return std::make_unique<MockTape>(); };
    }

    void runSortAndVerify(
        const std::vector<int32_t> &inputData,
        size_t memoryLimitBytes,
        double memoryUtilizationFactor = 0.7,
        bool rewind = false
    ) {
        auto input = std::make_unique<MockTape>(inputData);
        auto output = std::make_unique<MockTape>();

        auto *outputRaw = output.get();

        SorterConfig sorterConfig;
        sorterConfig.tapeFactory = factory_;
        sorterConfig.memoryLimitBytes = memoryLimitBytes;

        TapeSorter sorter(std::move(input), std::move(output), sorterConfig);

        ASSERT_TRUE(sorter.sort(rewind));

        std::vector<int32_t> result;
        int32_t val = 0;

        outputRaw->rewind();
        while (outputRaw->read(val)) {
            result.push_back(val);
            outputRaw->moveRight();
        }

        EXPECT_TRUE(std::is_sorted(result.begin(), result.end()));

        std::vector<int32_t> expected = inputData;
        std::sort(expected.begin(), expected.end());
        EXPECT_EQ(result, expected);
    }
};

// ---------- Basic scenarios ----------

TEST_F(TapeSorterTest, EmptyInput) {
    runSortAndVerify({}, 1024);
}

TEST_F(TapeSorterTest, SingleElement) {
    runSortAndVerify({ 42 }, 1024);
}

TEST_F(TapeSorterTest, AlreadySorted) {
    std::vector<int32_t> data(100);
    std::iota(data.begin(), data.end(), 0);
    runSortAndVerify(data, 1024);
}

TEST_F(TapeSorterTest, Reversed) {
    std::vector<int32_t> data(100);
    std::iota(data.rbegin(), data.rend(), 0);
    runSortAndVerify(data, 1024);
}

TEST_F(TapeSorterTest, RandomSmall) {
    std::vector<int32_t> data = { 5, 2, 8, 1, 9, 3, 7, 4, 6 };
    runSortAndVerify(data, 1024);
}

// ---------- Memory limit / chunking ----------

TEST_F(TapeSorterTest, MultipleChunks) {
    std::vector<int32_t> data(1000);
    std::iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), std::mt19937 { 42 });
    runSortAndVerify(data, 600);
}

TEST_F(TapeSorterTest, SingleElementChunks) {
    std::vector<int32_t> data = { 5, 3, 8, 1, 9, 2 };
    runSortAndVerify(data, sizeof(int32_t), 1.0);
}

TEST_F(TapeSorterTest, MemoryLargerThanData) {
    std::vector<int32_t> data = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    runSortAndVerify(data, 1024 * 1024);
}

// ---------- Data integrity ----------

TEST_F(TapeSorterTest, PreservesElements) {
    std::vector<int32_t> data(5000);
    std::iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), std::mt19937 { 12345 });
    runSortAndVerify(data, 2000);
}
