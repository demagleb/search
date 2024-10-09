#include <lsm/structures/memtable.hpp>
#include <lsm/structures/sstable.hpp>


namespace lsm::structures {

std::optional<structures::Row> MemTable::getByKey(const std::string& key) const
{
    auto iter = table_.find(key);
    if (iter == table_.end() || iter->second.empty()) {
        return std::nullopt;
    }
    return Row(iter->first, iter->second);
}

std::generator<structures::Row&> MemTable::getByKeyRange(
    const std::string& left, const std::string& right) const
{
    auto iter = table_.lower_bound(left);
    while (iter != table_.end() && iter->first <= right) {
        Row row(iter->first, iter->second);
        co_yield row;
        ++iter;
    }
}

std::generator<structures::Row&> MemTable::getAll() const
{
    for (const auto& [key, value] : table_) {
        Row row(key, value);
        co_yield row;
    }
}


namespace marshal::impl {

template <>
MemTable fromStreamImpl<MemTable>(std::istream& istream)
{
    SSTable sstable(istream);
    MemTable memtable;
    for (auto& row : sstable.getAll()) {
        memtable.insert(std::move(row));
    }
    return memtable;
}

template <>
std::streamoff toStreamImpl<MemTable>(const MemTable& value, std::ostream& ostream)
{
    auto pos = ostream.tellp();
    auto rows = [&] -> std::generator<Row&> {
        for (const auto& [k, v] : value.table()) {
            Row row(k, v);
            co_yield row;
        }
    }();

    writeSSTable(ostream, std::move(rows));
    return pos;
}

} // namespace marshal::impl

} // namespace lsm::structures
