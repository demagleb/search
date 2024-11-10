#include <cstdint>
#include <temporal_index/filter.hpp>
#include <temporal_index/temporal_index.hpp>

namespace temporal_index::filter {

namespace {

class ValidBetweenFilter : public IFilter {
public:
    ValidBetweenFilter(
        uint64_t begin,
        uint64_t end)
        : begin_(begin)
        , end_(end)
    { }

    ~ValidBetweenFilter() = default;

    roaring::Roaring64Map exec(bitmap_index::BitmapIndex& index) override final {
        temporal_index::TemporalIndex* temporalIndex = dynamic_cast<temporal_index::TemporalIndex*>(&index);

        if (temporalIndex == nullptr) {
            throw std::runtime_error("Can't cast index to TemporalIndex");
        }

        return temporalIndex->getWithLessEqualTimestamp(begin_, TemporalIndex::TimestampType::Begin) -
            temporalIndex->getWithLessEqualTimestamp(end_, TemporalIndex::TimestampType::End);
    }

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<ValidBetweenFilter>(begin_, end_);
    }

private:
    uint64_t begin_;
    uint64_t end_;
};

class CreatedBetweenFilter : public IFilter {
public:
    CreatedBetweenFilter(
        uint64_t begin,
        uint64_t end)
        : begin_(begin)
        , end_(end)
    { }

    ~CreatedBetweenFilter() = default;

    roaring::Roaring64Map exec(bitmap_index::BitmapIndex& index) override final {
        temporal_index::TemporalIndex* temporalIndex = dynamic_cast<temporal_index::TemporalIndex*>(&index);

        if (temporalIndex == nullptr) {
            throw std::runtime_error("Can't cast index to TemporalIndex");
        }

        return temporalIndex->getWithGreaterEqualTimestamp(begin_, TemporalIndex::TimestampType::Begin) &
            temporalIndex->getWithLessEqualTimestamp(end_, TemporalIndex::TimestampType::Begin);
    }

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<CreatedBetweenFilter>(begin_, end_);
    }

private:
    uint64_t begin_;
    uint64_t end_;
};

} // namespace

Filter validBetween(uint64_t begin, uint64_t end)
{
    return Filter(std::make_unique<ValidBetweenFilter>(begin, end));
}

Filter createdBetween(uint64_t begin, uint64_t end)
{
    return Filter(std::make_unique<CreatedBetweenFilter>(begin, end));
}

} // namespace temporal_index::filter
