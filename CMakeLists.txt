cmake_minimum_required(VERSION 3.20)
project(TatlinUnifiedTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/FileTape.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/conf.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)