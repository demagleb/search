#include <inverted_index/tokenize/tokenize.hpp>
#include <inverted_index/tokenize/stop_words.hpp>
#include <porter2_stemmer.h>

#include <cctype>

namespace inverted_index::tokenize {

std::generator<std::string&> tokenize(std::string_view doc)
{
    std::string word;
    for (char c: doc) {
        if (std::isspace(c) || std::ispunct(c)) {
            auto preparedWord = prepareWord(std::move(word));
            if (preparedWord) {
                co_yield *preparedWord;
            }
            word.clear();
        } else {
            word.push_back(c);
        }
    }
}

std::optional<std::string> prepareWord(std::string word)
{
    Porter2Stemmer::trim(word);
    if (!isStopWord(word) && !word.empty()) {
        Porter2Stemmer::stem(word);
        return std::move(word);
    }
    return std::nullopt;
}

} // namespace inverted_index::tokenizer
