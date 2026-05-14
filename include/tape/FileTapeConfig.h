#ifndef TATLINUNIFIEDTEST_TAPECONFIG_H
#define TATLINUNIFIEDTEST_TAPECONFIG_H

#include <chrono>
#include <filesystem>

struct FileTapeConfig {
    std::chrono::nanoseconds readDelay { 0 };
    std::chrono::nanoseconds writeDelay { 0 };
    std::chrono::nanoseconds moveDelay { 0 };
    std::chrono::nanoseconds rewindDelay { 0 };

    std::filesystem::path fileTapeConfPath;

    explicit FileTapeConfig(const std::filesystem::path &path = "");
};

#endif // TATLINUNIFIEDTEST_TAPECONFIG_H
