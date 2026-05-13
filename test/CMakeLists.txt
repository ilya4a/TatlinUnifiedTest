add_executable(TatlinUnifiedTest_test
        test_TapeSorter.cpp
        ${PROJECT_SOURCE_DIR}/src/sorting/TapeSorter.cpp

)

target_include_directories(TatlinUnifiedTest_test PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(TatlinUnifiedTest_test PRIVATE GTest::GTest GTest::Main)

include(GoogleTest)
gtest_discover_tests(TatlinUnifiedTest_test)