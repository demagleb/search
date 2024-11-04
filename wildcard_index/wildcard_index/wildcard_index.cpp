#include "bitmap_index/filter.hpp"
#include "fmt/base.h"
#include "roaring64map.hh"
#include <stdexcept>
#include <wildcard_index/wildcard_index.hpp>
#include <wildcard_index/tokenize.hpp>

#include <algorithm>

namespace wildcard_index {

namespace {

constexpr char START_OF_TERM = '$';
constexpr char END_OF_TERM = '#';

std::string escapeLeft(std::string word) {
    word.insert(0, 1, START_OF_TERM);
    return word;
}

std::string escapeRight(std::string word) {
    word.push_back(END_OF_TERM);
    return word;
}

std::string escape(std::string word) {
    return escapeRight(escapeLeft(std::move(word)));
}

} // namespace


void WildcardIndex::insertDoc(uint64_t key, const std::string& doc) {
    for (auto& word : tokenize(doc)) {
        word = escape(std::move(word));
        insert(word, key);
        std::reverse(word.begin(), word.end());
        insert(word, key);
    }
}

roaring::Roaring64Map WildcardIndex::getByWildcard(const std::string& wildcard) {
    auto startsCnt = std::count(wildcard.begin(), wildcard.end(), '*');
    if (startsCnt == 0) {
        return getByKey(escape(wildcard));
    }
    if (startsCnt > 1) {
        throw std::runtime_error("Do not support more than one '*'");
    }

    auto starPos = wildcard.find('*');

    std::string left = escapeLeft("");
    left.append(wildcard.begin(), wildcard.begin() + starPos);

    std::string right(wildcard.begin() + starPos + 1, wildcard.end());
    right = escapeRight(std::move(right));
    std::reverse(right.begin(), right.end());

    roaring::Roaring64Map bitmap;
    if (left.size() > 1 && right.size() > 1) {
        auto leftBitmap = bitmap_index::filter::between(left, left + "~").exec(*this);
        auto rightBitmap = bitmap_index::filter::between(right, right + "~").exec(*this);
        return leftBitmap & rightBitmap;
    }

    if (left.size() > 1) {
        return bitmap_index::filter::between(left, left + "~").exec(*this);
    }

    if (right.size() > 1) {
        return bitmap_index::filter::between(right, right + "~").exec(*this);
    }

    return bitmap_index::filter::between(std::string(1, START_OF_TERM), std::string(1, START_OF_TERM + 1)).exec(*this);
}

} // namespace wildcard_index
