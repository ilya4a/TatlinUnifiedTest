#include <atomic>
#include <iostream>
#include <random>

#include "sorting/TapeSorter.h"
#include "tape/FileTape.h"

const size_t TAPE_SIZE = 100;
namespace fs = std::filesystem;

void clearTempDirectory() {
    fs::path tmpDir = "./tmp";
    if (fs::exists(tmpDir) && fs::is_directory(tmpDir)) {
        for (const auto& entry : fs::directory_iterator(tmpDir)) {
            fs::remove_all(entry.path());
        }
    }
}

std::int32_t randomInt32() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<std::int32_t> dist(
        std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max()
    );
    return dist(gen)%100;
}

void fill(std::unique_ptr<FileTape>& source) {
    for (int i = 0; i < TAPE_SIZE; i++) {
        source->write(randomInt32());
        source->moveRight();
    }
    source->rewind();
}


int main() {
    std::filesystem::create_directories("./tmp");
    TapeConfig tape_config;

    std::unique_ptr<FileTape> source = std::make_unique<FileTape>(tape_config, "./tmp/source", true);
    std::unique_ptr<FileTape> output = std::make_unique<FileTape>(tape_config, "./tmp/output", true);

    fill(source);
    std::atomic<size_t> t{0};

    auto create_func = [&tape_config, &t](std::filesystem::path path) {
        std::filesystem::path name = path / std::string( "temp_tape" + std::to_string(t));
        t++;
        return std::make_unique<FileTape>(tape_config, name, true);
    };

    TapeSorter tape_sorter(std::move(source), std::move(output), create_func, TAPE_SIZE*sizeof(std::int32_t)/3);

    tape_sorter.sort();

    // clearTempDirectory();
    return 0;
}
