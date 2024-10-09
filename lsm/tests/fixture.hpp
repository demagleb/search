#include <lsm/lsm.hpp>
#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <random>

constexpr size_t MaxMemtableSize = 200;
constexpr size_t SparseIndexFrequency = 10;

namespace {

inline std::string dirName()
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, characters.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < 20; ++i) {
        randomString += characters[dist(gen)];
    }
    return randomString;
}

} // namespace

constexpr size_t KEY_SIZE = 10;

inline std::generator<lsm::Row&> generateRows(size_t size) {
    for (size_t num = 0; num < size; ++num) {
        auto snum = std::to_string(num);
        snum = std::string(KEY_SIZE - snum.size(), '0') + snum;
        lsm::Row row(snum, snum);
        co_yield row;
    }
}

class LSMFixture : public testing::Test {
public:
    LSMFixture()
        : dir_(std::filesystem::temp_directory_path() / dirName())
    {
        std::filesystem::create_directory(dir_);
        tree_ = std::make_unique<lsm::LSMTree>(dir_, MaxMemtableSize, SparseIndexFrequency);
    }

    ~LSMFixture()
    {
        tree_.reset();
        std::filesystem::remove_all(dir_);
    }

    lsm::LSMTree& lsmTree() { return *tree_; }

    const std::filesystem::path& dir() const { return dir_; }



private:
    const std::filesystem::path dir_;
    std::unique_ptr<lsm::LSMTree> tree_;
};

// LSMFixture<> getFixture() { return LSMFixture<>(); };
