#include "tape/FileTape.h"
#include <filesystem>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

TapeConfig::TapeConfig(std::filesystem::path const& path )
{
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
            tmp_path = value;
        }
    }
}


FileTape::FileTape(TapeConfig  config, std::filesystem::path  path, bool truncate):
path_(std::move(path)), config_(std::move(config)) {

    if (truncate) {
        std::ofstream create(path_, std::ios::binary | std::ios::trunc);
        if (!create) {
            throw std::runtime_error("Cannot create tape file: " + path_.string());
        }
    } else if (!fs::exists(path_)) {
        std::ofstream create(path_, std::ios::binary);
        if (!create) {
            throw std::runtime_error("Cannot create tape file: " + path_.string());
        }
    }

    std::ios::openmode mode = std::ios::binary | std::ios::in | std::ios::out;
    if (truncate) {
        mode |= std::ios::trunc;
    }

    file_.open(path_, mode);
    if (!file_) {
        throw std::runtime_error("Cannot open tape file: " + path_.string());
    }

    if (fs::exists(path_)) {
        const auto bytes = fs::file_size(path_);

        if (bytes % ElementSize != 0) {
            throw std::runtime_error("Invalid tape file size");
        }

        size_ = bytes / ElementSize;
    } else {
        size_ = 0;
    }

    position_ = 0;
    syncToPosition();
}

FileTape::~FileTape() {
    try {
        file_.flush();
        file_.close();
    } catch (...) {
        std::cerr << "Cannot close tape file: " + path_.string() << std::endl;
    }
}

bool FileTape::read(std::int32_t& value) {
    if (position_ >= size()) {
        return false;
    }

    syncToPosition();
    file_.read(reinterpret_cast<char*>(&value), sizeof(value));

    if (!file_) {
        return false;
    }

    sleep(config_.readDelay);
    return true;
}

void FileTape::write(std::int32_t const & value) {
    if (position_ > size_) {
        throw std::runtime_error("Attempt to write beyond end of tape");
    }
    syncToPosition();

    file_.write(reinterpret_cast<const char*>(&value), sizeof(value));

    if (!file_) {
        throw std::runtime_error("Failed to write to tape file: " + path_.string());
    }

    if (position_ == size_) {
        size_++;
    }

    file_.flush();
    sleep(config_.writeDelay);
}

bool FileTape::moveLeft() {
    if (position_ == 0) {
        return false;
    }
    position_--;
    sleep(config_.moveDelay);
    return true;
}

bool FileTape::moveRight() {
    if (position_ >= size_) {
        return false;
    }
    position_++;
    sleep(config_.moveDelay);
    return true;
}

void FileTape::rewind() {
    position_ = 0;
    sleep(config_.rewindDelay);
}

void FileTape::syncToPosition() {
    file_.clear();
    auto off = static_cast<std::streamoff>(position_ * ElementSize);

    file_.seekg(off, std::ios::beg);
    file_.seekp(off, std::ios::beg);
}

void FileTape::sleep(std::chrono::milliseconds delay) const {
    if (delay.count() > 0) {
        std::this_thread::sleep_for(delay);
    }
}

std::size_t FileTape::position() const {
    return position_;
}

std::size_t FileTape::size() const {
    return size_;
}


