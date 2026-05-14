

#ifndef TATLINUNIFIEDTEST_TAPECONFIG_H
#define TATLINUNIFIEDTEST_TAPECONFIG_H

#include <chrono>
#include <filesystem>

struct FileTapeConfig {
    std::chrono::milliseconds readDelay{0};
    std::chrono::milliseconds writeDelay{0};
    std::chrono::milliseconds moveDelay{0};
    std::chrono::milliseconds rewindDelay{0};

    std::filesystem::path fileTapeConfPath;

    explicit FileTapeConfig(std::filesystem::path const& path = "");
};

#endif // TATLINUNIFIEDTEST_TAPECONFIG_H
