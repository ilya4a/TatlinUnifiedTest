#ifndef TATLINUNIFIEDTEST_TAPEI_H
#define TATLINUNIFIEDTEST_TAPEI_H
#include <cstdint>

class TapeI {
public:
    virtual bool read(std::int32_t& value) = 0;
    virtual void write(std::int32_t const& value) = 0;

    virtual bool moveLeft() = 0;
    virtual bool moveRight() = 0;

    virtual std::size_t position() const = 0;
    virtual std::size_t size() const = 0;

    virtual void rewind() = 0;

    virtual ~TapeI() = default;

};

#endif // TATLINUNIFIEDTEST_TAPEI_H
