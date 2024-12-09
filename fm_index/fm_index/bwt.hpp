#pragma once

#include <string>

namespace fm_index::bwt {

std::string directTransform(const std::string& str);
std::string inverseTransform(std::string str);

} // namespace fm_index::bwt
