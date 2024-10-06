#pragma once

#include <lsm/structures/row.hpp>

#include <generator>

namespace lsm::structures {

class SSTable {
public:
    SSTable(std::istream& stream)
        : stream_(stream)
    { }

    void setPos(std::streampos pos);

    Row getRow();
    std::generator<Row&> getAll();

    bool end();
private:
    std::istream& stream_;
};

void writeSSTable(std::ostream& ostream, std::generator<Row&> rows);
std::generator<std::pair<std::string, std::streamoff>&> writeSSTableWithOffsets(
    std::ostream& ostream, std::generator<Row&> rows);

} // namespace lsm::structures
