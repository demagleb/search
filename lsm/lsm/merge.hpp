#include <lsm/structures/row.hpp>

#include <generator>
#include <vector>

namespace lsm {

std::generator<structures::Row&> merge(std::vector<std::generator<structures::Row&>> ranges, bool removeTombstones);

} // namespace lsm
