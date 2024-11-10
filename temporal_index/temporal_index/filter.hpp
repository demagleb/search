#include <bitmap_index/filter.hpp>

namespace temporal_index::filter {

using namespace bitmap_index::filter;

Filter validBetween(uint64_t begin, uint64_t end);
Filter createdBetween(uint64_t begin, uint64_t end);

} // namespace temporal_index::filter
