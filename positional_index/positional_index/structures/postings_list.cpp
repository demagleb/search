#include "roaring64map.hh"
#include <positional_index/structures/postings_list.hpp>

#include <iostream>
#include <fmt/format.h>

namespace positional_index::structures {

std::generator<Posting&> readPostingsList(std::string data)
{

    std::string_view str = data;
    while (str.size() > 0) {
        uint32_t position;
        std::memcpy(&position, str.data(), sizeof(position));
        str = str.substr(sizeof(position));

        size_t bitmapSize;
        std::memcpy(&bitmapSize, str.data(), sizeof(bitmapSize));

        str = str.substr(sizeof(bitmapSize));
        auto bitmap = roaring::Roaring64Map::readSafe(str.data(), bitmapSize);
        str = str.substr(bitmapSize);

        Posting posting(position, std::move(bitmap));
        co_yield posting;
    }
}

std::string writePostingsList(std::generator<Posting&> postings)
{
    std::string s;
    for (auto& posting : postings) {
        uint32_t position = posting.position();
        s.append(reinterpret_cast<const char*>(&position), sizeof(position));
        size_t bitmapSize = posting.bitmap().getSizeInBytes();
        s.append(reinterpret_cast<const char*>(&bitmapSize), sizeof(bitmapSize));
        s.append(bitmapSize, '\0');
        posting.bitmap().write(&*(s.end() - bitmapSize));
    }
    return s;
}

std::generator<Posting&> mergePostingsLists(std::generator<Posting&> lhs, std::generator<Posting&> rhs)
{
    auto leftIter = lhs.begin();
    auto rightIter = rhs.begin();
    Posting res(0, {});
    while (leftIter != lhs.end() || rightIter != rhs.end()) {
        if (leftIter == lhs.end()) {
            res = std::move(*rightIter);
            co_yield res;
            ++rightIter;
            continue;
        }
        if (rightIter == rhs.end()) {
            res = std::move(*leftIter);
            co_yield res;
            ++leftIter;
            continue;
        }
        if ((*leftIter).position() == (*rightIter).position()) {
            res = std::move(*leftIter);
            res.bitmap() |= (*rightIter).bitmap();
            co_yield res;
            ++leftIter;
            ++rightIter;
            continue;
        }
        if ((*leftIter).position() < (*rightIter).position()) {
            res = std::move(*leftIter);
            co_yield res;
            ++leftIter;
            continue;
        }
        res = std::move(*rightIter);
        co_yield res;
        ++rightIter;
    }
}


} // namespace positional_index::structures
