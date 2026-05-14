#include "tape/FileTapeConfig.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

FileTapeConfig::FileTapeConfig(std::filesystem::path const& path){

    std::filesystem::path configPath;
    if (path.empty()) {
        configPath = "./config/tape.conf";
    } else {
        configPath = path;
    }

    std::ifstream file(configPath);
    if (!file) {
        throw std::runtime_error("Cannot open config: " + configPath.string());
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value)) continue;

        if (key == "read_delay_ns") {
            readDelay = std::chrono::nanoseconds(std::stoll(value));
        } else if (key == "write_delay_ns") {
            writeDelay = std::chrono::nanoseconds(std::stoll(value));
        } else if (key == "move_delay_ns") {
            moveDelay = std::chrono::nanoseconds(std::stoll(value));
        } else if (key == "rewind_delay_ns") {
            rewindDelay = std::chrono::nanoseconds(std::stoll(value));
        }
    }
}
