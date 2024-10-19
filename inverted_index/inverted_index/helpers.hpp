#pragma once

#include "fmt/base.h"
#include <roaring64map.hh>
#include <string>

#include <fmt/format.h>

namespace inverted_index {

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

} // namespace inverted_index
