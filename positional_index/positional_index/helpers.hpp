#include <generator>
#include <set>

namespace positional_index {

template <typename T>
std::generator<T&> toGenerator(std::set<T>& values) {
    for (auto& value : values) {
        T v = value;
        co_yield v;
    }
}

} // namespace positional_index
