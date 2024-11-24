#pragma once

#include <compare>
#include <roaring64map.hh>
#include <generator>

namespace positional_index::structures {

class Posting {
public:
    Posting(uint32_t position, roaring::Roaring64Map bitmap)
        : position_(position)
        , bitmap_(std::move(bitmap))
    { }

    uint32_t position() const { return position_; }
    roaring::Roaring64Map& bitmap() { return bitmap_; };
    const roaring::Roaring64Map& bitmap() const { return bitmap_; };

    std::strong_ordering operator<=>(const Posting& other) const { return position_ <=> other.position_; }
private:
    int32_t position_;
    roaring::Roaring64Map bitmap_;
};

std::generator<Posting&> readPostingsList(std::string str);
std::string writePostingsList(std::generator<Posting&> entries);
std::generator<Posting&> mergePostingsLists(std::generator<Posting&> lhs, std::generator<Posting&> rhs);


} // namespace positional_index::structures
