#pragma once

#include <array>
#include <climits>
#include <string>
#include <vector>

namespace fm_index {

class FmIndex {
public:
    FmIndex(const std::string& text);

    size_t count(const std::string& pattern) const;
    std::vector<size_t> find(const std::string& pattern) const;

private:
    std::pair<size_t, size_t> findRange(const std::string& pattern) const;
    size_t l2f(size_t index) const;
    size_t occ(unsigned char c, size_t r) const;
    size_t locate(size_t index) const;

    std::array<size_t, UCHAR_MAX + 1> c_;
    std::string l_;
};

} // namespace fm_index
