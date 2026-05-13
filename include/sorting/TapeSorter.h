
#ifndef TATLINUNIFIEDTEST_TAPESORTER_H
#define TATLINUNIFIEDTEST_TAPESORTER_H
#include <filesystem>
#include <functional>
#include <memory>

#include "tape/TapeI.h"
#include "utils/MinHeap.h"

class TapeSorter {

    public:
        TapeSorter(std::unique_ptr<TapeI> input,
            std::unique_ptr<TapeI> output,
            std::function<std::unique_ptr<TapeI>(std::filesystem::path path)> create_tape_function,
            size_t memoryLimitBytes,
            double memoryUtilizationFactor = 0.7,
            std::filesystem::path tmp_dir = "./tmp");

        bool sort();

    private:
        std::unique_ptr<TapeI> input_;
        std::unique_ptr<TapeI> output_;

        size_t memoryLimitBytes_ {0};
        size_t maxChunkElements_ {0};

        std::function<std::unique_ptr<TapeI>(std::filesystem::path path)> create_tape;

        std::vector<std::unique_ptr<TapeI>> tempTapes_;

        std::filesystem::path tmp_dir_;


        bool createTempTapes();
        bool createTempTapesSeq();
        MinHeap<std::pair<std::int32_t, size_t>> fillTempHeap();
        bool runMerge();
    };


#endif // TATLINUNIFIEDTEST_TAPESORTER_H
