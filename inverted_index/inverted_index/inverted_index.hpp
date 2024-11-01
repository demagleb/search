#pragma once

#include <bitmap_index/bitmap_index.hpp>

namespace inverted_index {

class InvertedIndex : public bitmap_index::BitmapIndex {
public:
    void insertDoc(uint64_t key, const std::string& doc);
    roaring::Roaring64Map getByWord(std::string word);
};

} // namespace inverted_index
