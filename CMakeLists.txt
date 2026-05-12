cmake_minimum_required(VERSION 3.20)
project(TatlinUnifiedTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/tape/FileTape.cpp
        include/sorting/TapeSorter.h
        src/sorting/TapeSorter.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/tape.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)