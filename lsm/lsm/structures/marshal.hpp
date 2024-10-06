#pragma once

#include <iostream>
#include <ostream>

namespace lsm::structures::marshal {

namespace impl {

template <typename T>
T fromStreamImpl(std::istream& istream);

template <typename T>
std::streamoff toStreamImpl(const T& value, std::ostream& ostream);

} // namespace impl

template <typename T>
inline T fromStream(std::istream& istream) {
    return impl::fromStreamImpl<T>(istream);
}

template <typename T>
inline std::streamoff toStream(const T& value, std::ostream& ostream) {
    return impl::toStreamImpl<T>(value, ostream);
}

} // namespace lsm::storage
