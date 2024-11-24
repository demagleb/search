#pragma once

#include <lsm/structures/row.hpp>

#include <generator>
#include <vector>

namespace positional_index {

std::generator<lsm::structures::Row&> positionalMerge(std::vector<std::generator<lsm::structures::Row&>> ranges, bool removeTombstones);

} // namespace positional_index
