
#include "sorting/TapeSorter.h"

#include <algorithm>
#include <filesystem>
#include <future>
#include <iostream>
#include <utility>

#include "utils/MinHeap.h"
#include "utils/Queue.h"

TapeSorter::TapeSorter(std::unique_ptr<TapeI> input,
    std::unique_ptr<TapeI> output,
    std::function<std::unique_ptr<TapeI>(std::filesystem::path path)> create_tape_function,
    size_t memoryLimitBytes,
    double memoryUtilizationFactor,
    std::filesystem::path tmp_dir) :

    input_(std::move(input)),
    output_(std::move(output)),
    create_tape(std::move(create_tape_function)),
    memoryLimitBytes_(memoryLimitBytes),
    maxChunkElements_(static_cast<size_t>(memoryLimitBytes * memoryUtilizationFactor / (sizeof(int32_t) * 3))),
    tmp_dir_(std::move(tmp_dir)) {}


bool TapeSorter::sort() {
    std::filesystem::create_directories("./tmp");

    createTempTapes();

    std::int32_t v;
    while (tempTapes_[0]->read(v)) {
        std::cout << v << std::endl;
        tempTapes_[0]->moveRight();
    }

    tempTapes_[0]->rewind();

    while (tempTapes_[5]->read(v)) {
        std::cout << v << std::endl;
        tempTapes_[5]->moveRight();
    }

    tempTapes_[5]->rewind();

    return false;
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

                while (chunk.size() < maxChunkElements_ && input_->moveRight()) {
                    if (!input_->read(val)) break;
                    chunk.push_back(val);
                }

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

                std::unique_ptr<TapeI> tape = create_tape(tmp_dir_);

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

            while (chunk.size() < maxChunkElements_ && input_->moveRight()) {
                if (!input_->read(val)) break;
                chunk.push_back(val);
            }
            if (chunk.empty()) break;

            std::sort(chunk.begin(), chunk.end());

            auto tape = create_tape(tmp_dir_);
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
    MinHeap<std::pair<std::int32_t, size_t>> heap(tempTapes_.size());

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


bool TapeSorter::runMerge() {
    MinHeap<std::pair<std::int32_t, size_t>> heap = fillTempHeap();

    BoundedBlockingQueue<int32_t> extractedHeap(1);

    std::thread handleHeap([&heap, &extractedHeap]() {
        // extractedHeap.push();
            heap.extractMin();
    });

    return false;
}



