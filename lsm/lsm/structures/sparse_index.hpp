#include <lsm/structures/marshal.hpp>

#include <absl/container/btree_map.h>

namespace lsm::structures {

class SparseIndex {
public:
    SparseIndex(absl::btree_map<std::string, std::streamoff> index = {})
        : index_(std::move(index))
    { }

    std::streamoff getStartPos(const std::string& key) const;

    void emplace(std::string key, std::streamoff pos);

    const absl::btree_map<std::string, std::streamoff> index() const { return index_; }

private:
    absl::btree_map<std::string, std::streamoff> index_;
};

namespace marshal::impl {

template <>
SparseIndex fromStreamImpl<SparseIndex>(std::istream& istream);

template <>
std::streamoff toStreamImpl<SparseIndex>(const SparseIndex& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
