#include "lsm/structures/row.hpp"
#include <ios>
#include <lsm/structures/sstable.hpp>
#include <lsm/structures/marshal.hpp>

namespace lsm::structures {

void SSTable::setPos(std::streamoff pos)
{
    stream_.clear();
    stream_.seekg(pos, std::ios_base::beg);
}

Row SSTable::getRow()
{
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

        std::pair<std::string, std::streamoff> keyOffset{
            std::move(row.key()),
            offset};
        co_yield keyOffset;
    }
}

} // namespace lsm::structures
