#ifndef TATLINUNIFIEDTEST_FILETAPE_H
#define TATLINUNIFIEDTEST_FILETAPE_H

#include "TapeI.h"

#include <chrono>
#include <filesystem>
#include <fstream>

struct TapeConfig {
    std::chrono::milliseconds readDelay{0};
    std::chrono::milliseconds writeDelay{0};
    std::chrono::milliseconds moveDelay{0};
    std::chrono::milliseconds rewindDelay{0};

    std::filesystem::path tmp_path;

    explicit TapeConfig(std::filesystem::path const& path = "");
    };


class FileTape final : public TapeI {

public:
    FileTape(TapeConfig config, std::filesystem::path path, bool truncate);

    ~FileTape() override;

    bool read(std::int32_t& value) override;
    void write(std::int32_t const& value) override;

    bool moveLeft() override;
    bool moveRight() override;

    void rewind() override;
    void syncToPosition();

    std::size_t position() const override;
    std::size_t size() const override;

private:
    void sleep(std::chrono::milliseconds delay) const;

    static constexpr std::size_t ElementSize = sizeof(std::int32_t);

    std::filesystem::path path_;
    TapeConfig config_;
    std::fstream file_;

    std::size_t position_{0};
    std::size_t size_{0};
};


#endif // TATLINUNIFIEDTEST_FILETAPE_H
