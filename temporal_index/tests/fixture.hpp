#include <temporal_index/temporal_index.hpp>
#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <random>

constexpr size_t MaxMemtableSize = 10;
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

class TemporalIndexFixture : public testing::Test {
public:
    TemporalIndexFixture()
        : dir_(std::filesystem::temp_directory_path() / dirName())
    {
        std::filesystem::create_directory(dir_);
        index_ = std::make_unique<temporal_index::TemporalIndex>(dir_);
    }

    temporal_index::TemporalIndex& index() { return *index_; }

    const std::filesystem::path& dir() const { return dir_; }

    void finish() {
        index_->finish();
        index_.reset();
        std::filesystem::remove_all(dir_);
    }

private:
    const std::filesystem::path dir_;
    std::unique_ptr<temporal_index::TemporalIndex> index_;
};
