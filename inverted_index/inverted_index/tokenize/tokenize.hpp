#pragma once

#include <generator>

namespace inverted_index::tokenize {

std::generator<std::string&> tokenize(std::string_view doc);

std::optional<std::string> prepareWord(std::string word);

} // namespace inverted_index::tokenizer
