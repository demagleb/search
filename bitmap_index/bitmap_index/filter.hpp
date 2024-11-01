#pragma once

#include <bitmap_index/bitmap_index.hpp>
#include <roaring64map.hh>

namespace bitmap_index::filter {

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual roaring::Roaring64Map exec(BitmapIndex& index) = 0;
    virtual std::unique_ptr<IFilter> copy() = 0;
};

class Filter : public IFilter {
public:
    Filter() = delete;
    Filter(std::unique_ptr<IFilter> filter);
    Filter(const Filter&);
    Filter(Filter&&);

    Filter& operator=(const Filter&);
    Filter& operator=(Filter&&);

    Filter& operator&=(const Filter& other);
    Filter& operator|=(const Filter& other);
    Filter& operator-=(const Filter& other);

    Filter operator&(const Filter& other);
    Filter operator|(const Filter& other);
    Filter operator-(const Filter& other);

    roaring::Roaring64Map exec(BitmapIndex& index) final override {
        return filter_->exec(index);
    }

    std::unique_ptr<IFilter> copy() final override {
        return filter_->copy();
    }

private:
    std::unique_ptr<IFilter> filter_;
};

Filter have(std::string key);
Filter between(std::string left, std::string right);

} // namespace bitmap_index
