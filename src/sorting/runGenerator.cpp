#include "sorting/runGenerator.h"

#include <atomic>
#include <random>

#include "sorting/TapeSorter.h"
#include "tape/FileTape.h"


namespace runGenerator {

    namespace fs = std::filesystem;

    std::int32_t randomInt32() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<std::int32_t> dist(
            std::numeric_limits<std::int32_t>::min(),
            std::numeric_limits<std::int32_t>::max()
        );
        return dist(gen)%100;
    }

    void fillRandom(std::unique_ptr<FileTape>& source, size_t tapeSize) {
        for (int i = 0; i < tapeSize; i++) {
            source->write(randomInt32());
            source->moveRight();
        }
        source->rewind();
    }


    void clearDirectory(fs::path path) {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                fs::remove_all(entry.path());
            }
        }
    }

    void run(fs::path input_path, fs::path output_path) {
        const size_t TAPE_SIZE = 1000;

        fs::path tmpDirPath = "./tmp";

        std::filesystem::create_directories(tmpDirPath);
        TapeConfig tape_config;

        std::unique_ptr<FileTape> source = std::make_unique<FileTape>(tape_config, input_path, false);
        std::unique_ptr<FileTape> output = std::make_unique<FileTape>(tape_config, output_path, false);

        std::atomic<size_t> t{0};

        auto create_func = [&tape_config, &t]() {
            std::filesystem::path name =  "./tmp/temp_tape" + std::to_string(t);
            t++;
            return std::make_unique<FileTape>(tape_config, name, true);
        };

        SorterConfig sorterConfig;
        sorterConfig.tapeFactory = create_func;
        sorterConfig.memoryLimitBytes = (TAPE_SIZE * sizeof(std::int32_t))/3;


        TapeSorter tape_sorter(std::move(source), std::move(output), sorterConfig);

        tape_sorter.sort();

    }

    void run_simple() {
        const size_t TAPE_SIZE = 1000;

        fs::path tmpDirPath = "./tmp";
        fs::path resDir = "./res";
        fs::path input_filename = "input";
        fs::path output_filename = "output";
        std::filesystem::create_directories(resDir);

        std::filesystem::create_directories(tmpDirPath);

        TapeConfig tape_config;

        std::unique_ptr<FileTape> source = std::make_unique<FileTape>(tape_config, resDir/input_filename, true);
        std::unique_ptr<FileTape> output = std::make_unique<FileTape>(tape_config, resDir/output_filename, true);

        fillRandom(source, TAPE_SIZE);

        std::atomic<size_t> t{0};

        auto create_func = [&tape_config, &t]() {
            std::filesystem::path name = "./tmp/temp_tape" + std::to_string(t);
            t++;
            return std::make_unique<FileTape>(tape_config, name, true);
        };

        SorterConfig sorterConfig;
        sorterConfig.tapeFactory = create_func;
        sorterConfig.memoryLimitBytes = (TAPE_SIZE * sizeof(std::int32_t))/3;

        TapeSorter tape_sorter(std::move(source), std::move(output), sorterConfig);

        if (!tape_sorter.sort()) {
            std::cerr << "can't complete sort" << std::endl;
        }

        if (std::filesystem::exists(tmpDirPath) && std::filesystem::is_directory(tmpDirPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(tmpDirPath)) {
                std::filesystem::remove_all(entry.path());
            }
            std::error_code ec;
            std::filesystem::remove(tmpDirPath, ec);
        }
    }
}
