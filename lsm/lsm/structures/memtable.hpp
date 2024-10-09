#pragma once

#include <iterator>
#include <lsm/structures/marshal.hpp>
#include <lsm/structures/row.hpp>

#include <map>
#include <generator>

namespace lsm::structures {

class MemTable {
public:
    MemTable() = default;

    std::optional<Row> getByKey(const std::string& key) const;
    std::generator<Row&> getByKeyRange(const std::string& left, const std::string& right) const;
    std::generator<Row&> getAll() const;

    void insert(Row row) { table_.insert_or_assign(std::move(row.key()), std::move(row.value())); };

    const std::map<std::string, std::string>& table() const { return table_; }
private:
    std::map<std::string, std::string> table_;
};

namespace marshal::impl {

template <>
MemTable fromStreamImpl<MemTable>(std::istream& istream);

template <>
std::streamoff toStreamImpl<MemTable>(const MemTable& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
