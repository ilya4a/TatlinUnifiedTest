#include "sorting/TapeSorter.h"
#include  "sorting/runGenerator.h"
#include "CLI/CLI.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

int app(int argc, char** argv, bool& showTime) {
    runGenerator::createDefaultRandFileTape("input", 10000);

    CLI::App app{"External tape sorter"};

    std::string inputPath;
    app.add_option("-i,--input", inputPath,
                   "Path to the input tape file")
        ->required()
        ->check(CLI::ExistingFile);

    std::string outputPath;
    app.add_option("-o,--output", outputPath,
                   "Path to the output tape file")
        ->required();


    size_t memoryLimit = 0;
    app.add_option("-m,--memory-limit", memoryLimit,
                   "Memory limit in bytes (e.g. 1048576)")
        ->required();


    double memoryFactor = 0.7;
    app.add_option("--memory-factor", memoryFactor,
                   "Memory utilization factor (0..1)")
        ->capture_default_str();

    std::string tmpDir = "./tmp";
    app.add_option("--tmp-dir", tmpDir,
                   "Directory for temporary tapes")
        ->capture_default_str();

    std::string tapeConfPath;
    app.add_option("--tape-config", tapeConfPath,
                   "Path to tape config file (optional)");

    bool runSeq = false;
    app.add_flag("--seq", runSeq,
                 "Use sequential sorting instead of parallel");

    int printLimit = -1;
    auto *print_opt = app.add_option("-p,--print-result", printLimit,
                       "Print sorted output to console (default 100 values)")
        ->expected(0, 1);

    app.add_flag("-t,--time", showTime, "Print execution time");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if (print_opt->count() > 0 && printLimit == -1) {
        printLimit = 100;
    }

    SorterConfig sorterConfig;
    sorterConfig.memoryLimitBytes = memoryLimit;
    sorterConfig.memoryUtilizationFactor = memoryFactor;
    sorterConfig.tmpDir = tmpDir;
    sorterConfig.run_seq = runSeq;

    bool success = runGenerator::run(inputPath, outputPath, sorterConfig, tmpDir, tapeConfPath);

    if (!success) {
        std::cerr << "Sorting failed.\n";
        return EXIT_FAILURE;
    }

    if (printLimit > 0) {
        runGenerator::printFileTape(outputPath, printLimit);
    }

    return EXIT_SUCCESS;
}


int main(int argc, char** argv) {
    auto start = std::chrono::steady_clock::now();

    bool showTime = false;
    int res = app(argc, argv, showTime);

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (showTime) std::cout << "Time: " << elapsed_ms << " ms\n";

    return res;
}