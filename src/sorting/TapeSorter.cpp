
#include "sorting/TapeSorter.h"

#include <future>
#include <utility>

TapeSorter::TapeSorter(std::unique_ptr<TapeI> input,
    std::unique_ptr<TapeI> output,
    std::function<std::unique_ptr<TapeI>()> create_tape_function,
    size_t memoryLimitBytes,
    double memoryUtilizationFactor = 0.7) :

    input_(std::move(input)),
    output_(std::move(output)),
    create_tape(std::move(create_tape_function)),
    memoryLimitBytes_(memoryLimitBytes),
    maxChunkElements_(static_cast<size_t>(memoryLimitBytes * memoryUtilizationFactor / sizeof(int32_t))){

}


bool TapeSorter::createTempTapes() {

    std::int32_t value;
    std::vector<std::int32_t> chunk(maxChunkElements_);

    int i = 0;
    do {
        input_->read(chunk[i]);

        if (input_->size() == i + 1) {

        }

        if (i + 1 == maxChunkElements_) {

        }

        i++;
    }
    while (input_->moveRight());
}


