#pragma once

#include <lsm/structures/marshal.hpp>

#include <string>

namespace lsm::structures {

class Row {
public:
    Row() = default;

    Row (std::string key, std::string value)
        : key_(std::move(key))
        , value_(std::move(value))
    { }

    const std::string& key() const { return key_; }
    std::string& key() { return key_; }

    const std::string& value() const { return value_; }
    std::string& value() { return value_; }

private:
    std::string key_;
    std::string value_;
};

namespace marshal::impl {

template <>
Row fromStreamImpl<Row>(std::istream& istream);

template <>
std::streamoff toStreamImpl<Row>(const Row& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
