#ifndef TATLINUNIFIEDTEST_RUNGENERATOR_H
#define TATLINUNIFIEDTEST_RUNGENERATOR_H
#include <filesystem>


namespace runGenerator {

    void run(std::filesystem::path input, std::filesystem::path output);
    void run_simple();
}

#endif // TATLINUNIFIEDTEST_RUNGENERATOR_H
