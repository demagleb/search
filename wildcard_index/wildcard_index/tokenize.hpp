#pragma once

#include <generator>

namespace wildcard_index {

std::generator<std::string&> tokenize(std::string_view doc);

std::string prepareWord(std::string word);

} // namespace wildcard_index
