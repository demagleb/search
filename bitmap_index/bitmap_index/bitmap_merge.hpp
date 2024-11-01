#pragma once

#include <lsm/structures/row.hpp>

#include <generator>
#include <vector>

namespace bitmap_index {

std::generator<lsm::structures::Row&> bitmapMerge(std::vector<std::generator<lsm::structures::Row&>> ranges, bool removeTombstones);

} // namespace bitmap_index
