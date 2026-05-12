
#ifndef TATLINUNIFIEDTEST_TAPESORTER_H
#define TATLINUNIFIEDTEST_TAPESORTER_H
#include <functional>
#include <memory>

#include "tape/TapeI.h"


class TapeSorter {

public:
    TapeSorter(std::unique_ptr<TapeI> input,
        std::unique_ptr<TapeI> output,
        std::function<std::unique_ptr<TapeI>()> create,
        size_t memoryLimitBytes);

    bool sort();

private:
    bool createTempTapes();
    bool runMerge();
};


#endif // TATLINUNIFIEDTEST_TAPESORTER_H
