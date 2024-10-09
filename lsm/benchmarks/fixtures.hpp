
#include <benchmark/benchmark.h>
#include <filesystem>
#include <lsm/lsm.hpp>

#include <memory>
#include <random>

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

// Row size = 64 bytes
constexpr size_t KEY_SIZE = 38;

inline std::generator<lsm::Row&> generateRows(size_t size) {
    for (size_t num = 0; num < size; ++num) {
        auto snum = std::to_string(num);
        snum = std::string(KEY_SIZE - snum.size(), '0') + std::move(snum);
        lsm::Row row(snum, snum);
        co_yield row;
    }
}

} // namespace

template<size_t InsertedElements = 0>
class LSMFixture : public benchmark::Fixture {
public:
    void SetUp(::benchmark::State& state) {
        dir_ = std::filesystem::temp_directory_path() / dirName();
        std::filesystem::create_directory(dir_);
        tree_ = std::make_unique<lsm::LSMTree>(dir_);

        tree_->insert(generateRows(InsertedElements));
        state.SetBytesProcessed(InsertedElements * 64);
    }

    void TearDown(::benchmark::State& state) {
        tree_.reset();
        std::filesystem::remove_all(dir_);
    }

    lsm::LSMTree& lsmTree() { return *tree_; }
private:
    std::filesystem::path dir_;
    std::unique_ptr<lsm::LSMTree> tree_;
};
