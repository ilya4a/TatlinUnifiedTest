#ifndef TATLINUNIFIEDTEST_RUNGENERATOR_H
#define TATLINUNIFIEDTEST_RUNGENERATOR_H
#include <filesystem>

#include "TapeSorter.h"


namespace runGenerator {
    namespace fs = std::filesystem;

    bool run(fs::path input_path, fs::path output_path, SorterConfig sorterConfig,
       fs::path tmpDirPath = "./tmp",
       fs::path fileTapeConfPath = "");

    void run_simple();

    void createDefaultRandFileTape(fs::path path, size_t size);
    int printFileTape(fs::path outputPath, size_t lim);
}

#endif // TATLINUNIFIEDTEST_RUNGENERATOR_H
