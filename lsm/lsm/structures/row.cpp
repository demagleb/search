#include <lsm/structures/row.hpp>

#include <cstdint>

namespace lsm::structures {

namespace marshal::impl {

// uint32  | uint32    | key_size bytes| value_size bytes
// keySize | valueSize | key           | value


template <>
Row fromStreamImpl<Row>(std::istream& istream)
{
    uint32_t keySize = 0;
    istream.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
    uint32_t valueSize = 0;
    istream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));

    std::string key;
    key.resize(keySize);
    istream.read(key.data(), keySize);

    std::string value;
    value.resize(valueSize);
    istream.read(value.data(), valueSize);

    return Row(std::move(key), std::move(value));
}

template <>
std::streamoff toStreamImpl<Row>(const Row& value, std::ostream& ostream)
{
    auto pos = ostream.tellp();
    uint32_t keySize = value.key().size();
    uint32_t valueSize = value.value().size();
    ostream << std::string_view(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
    ostream << std::string_view(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
    ostream << value.key();
    ostream << value.value();
    // std::cerr << "row write: " << pos << std::endl;
    return pos;
}

} // namespace marshal::impl

} // namespace lsm::structures
