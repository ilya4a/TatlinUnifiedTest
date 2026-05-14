#include "sorting/TapeSorter.h"

#include <algorithm>
#include <filesystem>
#include <future>
#include <utility>

#include "utils/MinHeap.h"
#include "utils/Queue.h"

TapeSorter::TapeSorter(std::unique_ptr<TapeI> input,
           std::unique_ptr<TapeI> output,
           SorterConfig config) :

    input_(std::move(input)),
    output_(std::move(output)),
    create_tape(std::move(config.tapeFactory)),
    memoryLimitBytes_(config.memoryLimitBytes),
    tmpDir_(std::move(config.tmpDir))
{
    if (config.run_seq) {
        maxChunkElements_ = static_cast<size_t>(config.memoryLimitBytes * config.memoryUtilizationFactor / sizeof(int32_t));
    }else {
        maxChunkElements_ = static_cast<size_t>(config.memoryLimitBytes * config.memoryUtilizationFactor / (sizeof(int32_t) * memory_divide_coef));
    }

}


bool TapeSorter::sort(bool rewind_tapes) {
    std::filesystem::create_directories(tmpDir_);

    if (!createTempTapes()) return false;

    runMerge();

    std::int32_t v;
    if (rewind_tapes) {
        output_->rewind();
        input_->rewind();
    }

    return true;
}

bool TapeSorter::sortSeq(bool rewind_tapes) {
    std::filesystem::create_directories(tmpDir_);

    if (!createTempTapesSeq()) return false;

    runMerge();
    std::int32_t v;

    return true;
}


TapeSorter::~TapeSorter() {

}

bool TapeSorter::createTempTapes() {
    BoundedBlockingQueue<std::vector<int32_t>> rawBlocks(1);
    BoundedBlockingQueue<std::vector<int32_t>> sortedBlocks(1);

    std::vector<std::unique_ptr<TapeI>> tempTapes;

    std::atomic<bool> errorOccurred{false};

    std::thread reader([&]() {
        try {
            while (true) {
                std::vector<int32_t> chunk;
                chunk.reserve(maxChunkElements_);

                int32_t val;
                if (!input_->read(val)) break;
                chunk.push_back(val);

                while (chunk.size() < maxChunkElements_) {
                    if (!input_->moveRight()) break;

                    if (!input_->read(val)) break;
                    chunk.push_back(val);
                }
                input_->moveRight();

                if (chunk.empty()) break;
                rawBlocks.push(std::move(chunk));
            }
            rawBlocks.close();
        } catch (...) {
            errorOccurred = true;
            rawBlocks.close();
        }
    });

    std::thread sorter([&]() {
        try {
            while (true) {
                std::vector<int32_t> block;
                if (!rawBlocks.pop(block)) break;
                std::sort(block.begin(), block.end());

                if (errorOccurred) break;
                sortedBlocks.push(std::move(block));
            }
            sortedBlocks.close();
        } catch (...) {
            errorOccurred = true;
            sortedBlocks.close();
        }
    });

    std::thread writer([&]() {
        try {
            while (true) {
                std::vector<int32_t> sortedBlock;
                if (!sortedBlocks.pop(sortedBlock)) break;
                if (errorOccurred) break;

                std::unique_ptr<TapeI> tape = create_tape();

                for (size_t i = 0; i < sortedBlock.size(); i++) {
                    tape->write(sortedBlock[i]);
                    tape->moveRight();
                }

                tape->rewind();
                tempTapes.push_back(std::move(tape));
            }
            sortedBlocks.close();
        } catch (...) {
            errorOccurred = true;
            sortedBlocks.close();
        }
    });

    reader.join();
    sorter.join();
    writer.join();

    if (errorOccurred) {
        return false;
    }

    tempTapes_ = std::move(tempTapes);
    return true;
}

bool TapeSorter::createTempTapesSeq() {
    try {
        std::vector<std::unique_ptr<TapeI>> tempTapes;

        while (true) {
            std::vector<int32_t> chunk;
            chunk.reserve(maxChunkElements_);

            int32_t val;
            if (!input_->read(val)) break;
            chunk.push_back(val);

            while (chunk.size() < maxChunkElements_) {
                if ( !input_->moveRight()) break;
                if (!input_->read(val)) break;
                chunk.push_back(val);
            }
            input_->moveRight();

            if (chunk.empty()) break;

            std::sort(chunk.begin(), chunk.end());

            auto tape = create_tape();
            if (!tape) {
                throw std::runtime_error("create_tape returned nullptr");
            }

            for (size_t i = 0; i < chunk.size(); ++i) {
                tape->write(chunk[i]);
                if (i + 1 < chunk.size()) {
                    tape->moveRight();
                }
            }
            tape->rewind();
            tempTapes.push_back(std::move(tape));
        }
        tempTapes_ = std::move(tempTapes);
        return true;
    } catch (...) {
        return false;
    }
}

MinHeap<std::pair<std::int32_t, size_t>> TapeSorter::fillTempHeap() {

    MinHeap<std::pair<std::int32_t, size_t>> heap(tempTapes_.size(),[](const std::pair<int32_t, size_t>& a,
        const std::pair<int32_t, size_t>& b) {
    return a.first < b.first;
    });

    std::int32_t value;
    size_t index = 0;
    for (auto &i: tempTapes_) {
        if (i->read(value)) {
            heap.insertNode(std::pair(value, index));
            i->moveRight();
        }
        index++;
    }
    return heap;
}


void TapeSorter::runMerge() {
    auto heap = fillTempHeap();

    while (true) {
        if (heap.empty()) break;

        std::pair<std::int32_t, size_t> min_value = heap.extractMin();
        output_->write(min_value.first);
        output_->moveRight();

        std::int32_t new_value = 0;
        if (tempTapes_[min_value.second]->read(new_value)) {

            heap.insertNode({new_value, min_value.second});
            // std::cout << "new value: " <<  new_value << std::endl;
            tempTapes_[min_value.second]->moveRight();
        }
    }

}
