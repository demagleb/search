#include <wildcard_index/tokenize.hpp>
#include <wildcard_index/stop_words.hpp>

#include <cctype>
#include <algorithm>

namespace wildcard_index {

std::generator<std::string&> tokenize(std::string_view doc)
{
    std::string word;
    for (char c: doc) {
        if (std::isspace(c) || std::ispunct(c)) {
            auto preparedWord = prepareWord(std::move(word));
            co_yield preparedWord;
            word.clear();
        } else {
            word.push_back(c);
        }
    }
}

std::string prepareWord(std::string word)
{
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    auto it = std::remove_if(word.begin(), word.end(), [](char ch) {
        return !((ch >= 'a' && ch <= 'z') || ch == '\'');
    });

    word.erase(it, word.end());
    return word;
}

} // namespace wildcard_index
