#include "fmt/base.h"
#include <kd_tree/bbox.hpp>
#include <stdexcept>
#include <cmath>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <cfloat>

namespace kd_tree {

void BBox::add(const std::vector<double>& vec)
{
    if (l_.empty()) {
        l_ = vec;
        r_ = vec;
        return;
    }
    if (vec.size() != l_.size()) {
        throw std::runtime_error(fmt::format("add: Different vector dimension {} != {}",
            vec.size(), l_.size()));
    }
    for (size_t i = 0; i < l_.size(); ++i) {
        l_[i] = std::min(l_[i], vec[i]);
        r_[i] = std::max(r_[i], vec[i]);
    }
}

void BBox::add(const BBox& bbox)
{
    if (bbox.l_.empty()) {
        return;
    }

    if (l_.empty()) {
        l_ = bbox.l_;
        r_ = bbox.r_;
        return;
    }
    if (bbox.l_.size() != l_.size()) {
        throw std::runtime_error(fmt::format("add: Different vector dimension {} != {}",
            bbox.l_.size(), l_.size()));
    }
    for (size_t i = 0; i < l_.size(); ++i) {
        l_[i] = std::min(l_[i], bbox.l_.at(i));
        r_[i] = std::max(r_[i], bbox.r_.at(i));
    }
}

double BBox::distance2(const std::vector<double>& vec) const
{
    if (l_.empty()) {
        return DBL_MAX;
    }
    if (vec.size() != l_.size()) {
        throw std::runtime_error(fmt::format("distance2: Different vector dimension {} != {}",
            vec.size(), l_.size()));
    }
    double distance2 = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (l_.at(i) <= vec[i] && vec[i] <= r_.at(i)) {
            continue;
        }
        double diff = std::min(std::abs(l_.at(i) - vec[i]), std::abs(r_.at(i) - vec[i]));
        distance2 += diff * diff;
    }
    return distance2;
}

} // namespace kd_tree
