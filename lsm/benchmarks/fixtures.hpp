
#include <benchmark/benchmark.h>
#include <filesystem>
#include <lsm/lsm.hpp>

#include <memory>
#include <random>
#include <string>

inline std::string randStr(size_t size)
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, characters.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < size; ++i) {
        randomString += characters[dist(gen)];
    }
    return randomString;
}

template<
    size_t MemtableSize = 1e6,
    size_t SparseIndexFrequency = 100,
    size_t InsertedElements = 0,
    size_t ValueSize = 10>
class LSMFixture : public benchmark::Fixture {
public:
    static constexpr size_t RowSize = ValueSize + 16;

    void SetUp(::benchmark::State& state) {
        dir_ = std::filesystem::temp_directory_path() / randStr(20);
        std::filesystem::create_directory(dir_);
        tree_ = std::make_unique<lsm::LSMTree>(dir_, MemtableSize, SparseIndexFrequency);

        tree_->insert(generateRows(InsertedElements));
    }

    void TearDown(::benchmark::State& state) {
        tree_.reset();
        std::filesystem::remove_all(dir_);
    }

    std::generator<lsm::Row&> generateRows(size_t size) {
        for (size_t num = 0; num < size; ++num) {
            std::string snum(reinterpret_cast<char*>(&num), sizeof(num));
            lsm::Row row(std::move(snum), randStr(ValueSize));
            co_yield row;
        }
    }

    std::string randKey () {
        std::uniform_int_distribution<> dist(0, InsertedElements);
        size_t num = dist(gen);
        return std::string(reinterpret_cast<char*>(&num), sizeof(num));
    }

    std::pair<std::string, std::string> randKeyRange() {
        std::uniform_int_distribution<> dist(0, InsertedElements);
        size_t lnum = dist(gen);
        size_t rnum = lnum + 100;
        std::string lk(reinterpret_cast<char*>(&lnum), sizeof(lnum));
        std::string rk(reinterpret_cast<char*>(&rnum), sizeof(rnum));
        return std::make_pair(std::move(lk), std::move(rk));
    }

    lsm::LSMTree& lsmTree() { return *tree_; }
private:
    std::filesystem::path dir_;
    std::mt19937_64 gen;
    std::unique_ptr<lsm::LSMTree> tree_;
};
