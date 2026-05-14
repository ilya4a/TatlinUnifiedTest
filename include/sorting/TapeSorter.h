#ifndef TATLINUNIFIEDTEST_TAPESORTER_H
#define TATLINUNIFIEDTEST_TAPESORTER_H

#include <filesystem>
#include <functional>
#include <memory>

#include "tape/TapeI.h"
#include "utils/MinHeap.h"

struct SorterConfig {
    size_t memoryLimitBytes;
    double memoryUtilizationFactor = 0.7;
    std::filesystem::path tmpDir = "./tmp";
    std::function<std::unique_ptr<TapeI>()> tapeFactory;
    bool run_seq = false;
};

class TapeSorter {
    static constexpr size_t memory_divide_coef = 3;

  public:
    TapeSorter(std::unique_ptr<TapeI> input, std::unique_ptr<TapeI> output, SorterConfig config);
    bool sort(bool rewind_tapes = true);
    bool sortSeq(bool rewind_tapes);

    ~TapeSorter();

  private:
    std::unique_ptr<TapeI> input_;
    std::unique_ptr<TapeI> output_;

    size_t memoryLimitBytes_ { 0 };
    size_t maxChunkElements_ { 0 };

    std::function<std::unique_ptr<TapeI>()> create_tape;

    std::vector<std::unique_ptr<TapeI>> tempTapes_;

    std::filesystem::path tmpDir_;

    bool createTempTapes();
    bool createTempTapesSeq();
    MinHeap<std::pair<std::int32_t, size_t>> fillTempHeap();
    void runMerge();
};

#endif // TATLINUNIFIEDTEST_TAPESORTER_H
