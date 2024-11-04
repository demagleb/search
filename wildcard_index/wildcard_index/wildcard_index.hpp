#pragma once

#include <bitmap_index/bitmap_index.hpp>

namespace wildcard_index {

class WildcardIndex : public bitmap_index::BitmapIndex {
public:
    WildcardIndex(std::filesystem::path dir)
        : bitmap_index::BitmapIndex(std::move(dir))
    { }

    void insertDoc(uint64_t key, const std::string& doc);
    roaring::Roaring64Map getByWildcard(const std::string& doc);
};

} // namespace wildcard_index
