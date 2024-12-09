#include <fm_index/bwt.hpp>

#include <climits>
#include <numeric>
#include <algorithm>
#include <vector>

namespace fm_index::bwt {

std::string directTransform(const std::string& str)
{
    std::vector<size_t> positions(str.size() + 1);
    std::iota(positions.begin(), positions.end(), 0ul);
    std::sort(positions.begin(), positions.end(), [&](auto lhs, auto rhs) {
        for (size_t _ = 0; _ < str.size(); _++) {
            if (lhs == str.size()) {
                return true;
            }
            if (rhs == str.size()) {
                return false;
            }
            if (str[lhs] != str[rhs]) {
                return static_cast<unsigned char>(str[lhs]) < static_cast<unsigned char>(str[rhs]);
            }
            lhs = (lhs + 1) % positions.size();
            rhs = (rhs + 1) % positions.size();
        }
        return false;
    });
    std::string result(positions.size(), '\0');
    for (size_t i = 0; i < positions.size(); ++i) {
        auto idx = (positions[i] + str.size()) % (str.size() + 1);
        result[i] = idx == str.size()
            ? '\0'
            : str[idx];
    }
    return result;
}

std::string inverseTransform(std::string str)
{
    std::vector<size_t> permutation(str.size());
    std::iota(permutation.begin(), permutation.end(), 0ul);
    std::stable_sort(permutation.begin(), permutation.end(), [&](auto lhs, auto rhs) {
        return static_cast<unsigned char>(str[lhs]) < static_cast<unsigned char>(str[rhs]);
    });
    auto applyPermutation = [&](const auto& permutation, const auto& seq) {
        auto newSeq = seq;
        for (size_t i = 0; i < permutation.size(); ++i) {
            newSeq[i] = seq[permutation[i]];
        }
        return newSeq;
    };

    std::string res(str.size(), '\0');
    for (size_t i = 0; i < str.size(); ++i) {
        str = applyPermutation(permutation, str);
        res[i] = str.front();
    }
    res.erase(0, 1);
    return res;
}

} // namespace namespace fm_index::bwt
