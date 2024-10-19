#pragma once

#include <roaring64map.hh>
#include <inverted_index/inverted_index.hpp>

namespace inverted_index {

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual roaring::Roaring64Map get(InvertedIndex& index) = 0;
    virtual std::unique_ptr<IFilter> copy() = 0;
};

class Filter : public IFilter {
public:
    Filter() = delete;
    Filter(const Filter&);
    Filter(Filter&&);

    Filter& operator=(const Filter&);
    Filter& operator=(Filter&&);


    static Filter have(std::string term);

    Filter& operator&=(const Filter& other);
    Filter& operator|=(const Filter& other);
    Filter& operator-=(const Filter& other);

    Filter operator&(const Filter& other);
    Filter operator|(const Filter& other);
    Filter operator-(const Filter& other);

    roaring::Roaring64Map get(InvertedIndex& index) final override {
        return filter_->get(index);
    }

    std::unique_ptr<IFilter> copy() final override {
        return filter_->copy();
    }

private:
    Filter(std::unique_ptr<IFilter> filter)
        : filter_(std::move(filter))
    { }

    std::unique_ptr<IFilter> filter_;
};

} // namespace inverted_index
