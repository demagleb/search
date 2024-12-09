#include <fm_index/bwt.hpp>
#include <fm_index/fm_index.hpp>
#include <fmt/format.h>
#include <ranges>
#include <algorithm>

namespace fm_index {

FmIndex::FmIndex(const std::string& text)
{
    l_ = bwt::directTransform(text);
    c_.fill(0);
    for (char c : l_) {
        for (size_t i = static_cast<unsigned char>(c) + 1; i < c_.size(); ++i) {
            ++c_[i];
        }
    }
}

size_t FmIndex::occ(unsigned char c, size_t r) const
{
    return std::count(l_.begin(), l_.begin() + std::min(r, l_.size()), c);
}

size_t FmIndex::l2f(size_t index) const
{
    return c_[l_[index]] + occ(l_[index], index);
}

std::pair<size_t, size_t> FmIndex::findRange(const std::string& pattern) const
{
    size_t l = 0;
    size_t r = l_.size();
    for (char a : pattern | std::views::reverse) {
        unsigned char ua = static_cast<unsigned char>(a);
        l = c_[ua] + occ(ua, l);
        r = c_[ua] + occ(ua, r);
        if (l == r) {
            return {0, 0};
        }
    }
    return {l, r};
}

size_t FmIndex::locate(size_t index) const
{
    index = l2f(index);
    size_t pos = 0;
    while (index != 0) {
        index = l2f(index);
        pos++;
    }
    return pos;
}

size_t FmIndex::count(const std::string& pattern) const
{
    auto [l, r] = findRange(pattern);
    return r - l;
}

std::vector<size_t> FmIndex::find(const std::string& pattern) const
{
    std::vector<size_t> indexes;
    auto [l, r] = findRange(pattern);
    while (l != r) {
        indexes.push_back(locate(l));
        ++l;
    }
    std::sort(indexes.begin(), indexes.end());
    return indexes;
}

} // namespace fm_index
