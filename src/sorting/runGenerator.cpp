#include "sorting/runGenerator.h"

#include <atomic>
#include <random>

#include "sorting/TapeSorter.h"
#include "tape/FileTape.h"


namespace runGenerator {


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

    void createDefaultRandFileTape(fs::path path) {
        std::unique_ptr<FileTape> source = std::make_unique<FileTape>(FileTapeConfig{}, path, true);
        fillRandom(source, 1000);
    }


    void clearDirectory(fs::path path) {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                fs::remove_all(entry.path());
            }
        }
    }

    void deleteDirectory(fs::path tmpDirPath) {
        if (fs::exists(tmpDirPath) && fs::is_directory(tmpDirPath)) {
            for (const auto& entry : fs::directory_iterator(tmpDirPath)) {
                fs::remove_all(entry.path());
            }
            std::error_code ec;
            fs::remove(tmpDirPath, ec);
        }
    }

    int printFileTape(fs::path outputPath, size_t lim) {

        std::ifstream in(outputPath, std::ios::binary);
        if (!in) {
            std::cerr << "Cannot open output file for reading.\n";
            return EXIT_FAILURE;
        }

        std::cout << "Sorted output:\n";
        int32_t val = 0;
        int count = 0;
        while (count < lim && in.read(reinterpret_cast<char*>(&val), sizeof(val)) ) {
            std::cout << val;
            ++count;
            if (count % 10 == 0) {
                std::cout << '\n';
            } else {
                std::cout << ' ';
            }
        }
        if (count % 10 != 0) {
            std::cout << '\n';
        }

        return 0;
    }

    bool run(fs::path input_path, fs::path output_path, SorterConfig sorterConfig,
        fs::path tmpDirPath,
        fs::path fileTapeConfPath
        ) {

        std::filesystem::create_directories(tmpDirPath);
        FileTapeConfig tapeConfig;

        tapeConfig.fileTapeConfPath = fileTapeConfPath;

        std::unique_ptr<FileTape> source = std::make_unique<FileTape>(tapeConfig, input_path, false);
        std::unique_ptr<FileTape> output = std::make_unique<FileTape>(tapeConfig, output_path, true);

        std::atomic<size_t> t{0};

        auto create_func = [tapeConfig, &t, tmpDirPath]() {
            std::filesystem::path name = tmpDirPath / ("temp_tape" + std::to_string(t));
            t++;
            return std::make_unique<FileTape>(tapeConfig, name, true);
        };

        sorterConfig.tapeFactory = create_func;

        TapeSorter tape_sorter(std::move(source), std::move(output), sorterConfig);

        bool res = tape_sorter.sort();

        deleteDirectory(tmpDirPath);
        return res;
    }


    void run_simple() {
        const size_t TAPE_SIZE = 1000;

        fs::path tmpDirPath = "./tmp";
        fs::path resDir = "./res";
        fs::path input_filename = "input";
        fs::path output_filename = "output";
        std::filesystem::create_directories(resDir);

        std::filesystem::create_directories(tmpDirPath);

        FileTapeConfig tape_config;

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
