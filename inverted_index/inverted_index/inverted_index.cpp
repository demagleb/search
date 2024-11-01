#include <inverted_index/inverted_index.hpp>
#include <inverted_index/tokenize.hpp>

namespace inverted_index {

void InvertedIndex::insertDoc(uint64_t key, const std::string& doc)
{
    auto words = tokenize(doc);
    for (auto& word : words) {
        insert(word, key);
    }
}

roaring::Roaring64Map InvertedIndex::getByWord(std::string word)
{
    auto preparedWord = prepareWord(word);
    if (!preparedWord) {
        return {};
    }
    return getByKey(*preparedWord);
}

} // namespace inverted_index
