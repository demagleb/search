#include <lsm/structures/row.hpp>

#include <generator>
#include <functional>

namespace lsm {

using MergeFunction = std::function<std::generator<structures::Row&>(std::vector<std::generator<structures::Row&>>, bool)>;

std::generator<structures::Row&> merge(std::vector<std::generator<structures::Row&>> ranges, bool removeTombstones);

} // namespace lsm
