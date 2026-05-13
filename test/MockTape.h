#ifndef TATLINUNIFIEDTEST_MOCKTAPE_H
#define TATLINUNIFIEDTEST_MOCKTAPE_H

#include "tape/TapeI.h"
#include <vector>
#include <stdexcept>

class MockTape : public TapeI {
public:

    explicit MockTape(const std::vector<std::int32_t>& data = {})
        : data_(data), pos_(0) {}

    bool read(std::int32_t& value) override {
        if (pos_ >= data_.size()) return false;
        value = data_[pos_];
        return true;
    }

    void write(std::int32_t const& value) override {
        if (pos_ > data_.size()) {
            throw std::runtime_error("Attempt to write beyond end of tape");
        }
        if (pos_ == data_.size()) {
            data_.resize(pos_ + 1);
        }
        data_[pos_] = value;
    }

    bool moveLeft() override {
        if (pos_ == 0) return false;
        --pos_;
        return true;
    }

    bool moveRight() override {
        if (pos_ >= data_.size()) return false;
        ++pos_;
        return true;
    }

    size_t position() const override {
        return pos_;
    }

    size_t size() const override {
        return data_.size();
    }

    void rewind() override {
        pos_ = 0;
    }

private:
    std::vector<std::int32_t> data_;
    std::size_t pos_;
};

#endif // TATLINUNIFIEDTEST_MOCKTAPE_H