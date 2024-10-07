#include "lsm/structures/row.hpp"
#include <ios>
#include <lsm/structures/sstable.hpp>
#include <lsm/structures/marshal.hpp>

namespace lsm::structures {

void SSTable::setPos(std::streamoff pos)
{
    // std::cerr << "sstable: offset " << stream_.tellg() << std::endl;
    // std::cerr << "sstable: set offset to " << pos << std::endl;
    stream_.clear();
    stream_.seekg(pos, std::ios_base::beg);
}

Row SSTable::getRow()
{
    // std::cerr << "getRow" << std::endl;
    return marshal::fromStream<Row>(stream_);
}

std::generator<Row&> SSTable::getAll()
{
    while (!end()) {
        Row row = getRow();
        co_yield row;
    }
}

bool SSTable::end()
{
    return stream_.peek() == EOF;
}

void writeSSTable(std::ostream& ostream, std::generator<Row&> rows)
{
    for (const auto& row : rows) {
        marshal::toStream(row, ostream);
    }
}

std::generator<std::pair<std::string, std::streamoff>&> writeSSTableWithOffsets(std::ostream& ostream, std::generator<Row&> rows)
{
    for (const auto& row : rows) {
        auto offset = marshal::toStream(row, ostream);
        // std::cerr << "sstable write key: " << row.key()
            // << " value: " << row.value()
            // << " offset: " << offset << std::endl;

        std::pair<std::string, std::streamoff> keyOffset{
            std::move(row.key()),
            offset};
        co_yield keyOffset;
    }
}

} // namespace lsm::structures
