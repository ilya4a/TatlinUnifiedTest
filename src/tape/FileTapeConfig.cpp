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

        if (key == "read_delay_ms") {
            readDelay = std::chrono::milliseconds(std::stoi(value));
        } else if (key == "write_delay_ms") {
            writeDelay = std::chrono::milliseconds(std::stoi(value));
        } else if (key == "move_delay_ms") {
            moveDelay = std::chrono::milliseconds(std::stoi(value));
        } else if (key == "rewind_delay_ms") {
            rewindDelay = std::chrono::milliseconds(std::stoi(value));
        } else if (key == "tmp_dir") {
            fileTapeConfPath = value;
        }
    }
}
