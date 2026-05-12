#include <iostream>

#include "tape/FileTape.h"

int main() {

    FileTape<std::int32_t> a(TapeConfig{}, "./config/tape.conf", true);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
