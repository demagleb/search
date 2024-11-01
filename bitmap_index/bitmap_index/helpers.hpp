#pragma once

#include <roaring64map.hh>
#include <string>

namespace bitmap_index {

inline roaring::Roaring64Map readBitmap(const std::string& bytes)
{
    auto bitmap = roaring::Roaring64Map::readSafe(bytes.data(), bytes.size());
    bitmap.runOptimize();
    return bitmap;
}

inline std::string writeBitmap(const roaring::Roaring64Map& bitmap)
{
    std::string bytes(bitmap.getSizeInBytes(), '\0');
    bitmap.write(bytes.data());
    return bytes;
}

} // namespace bitmap_index
