#pragma once

#include <string_view>

namespace inverted_index::tokenize {

bool isStopWord(std::string_view term);

} // namespace inverted_index::tokenizer
