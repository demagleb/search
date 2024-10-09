#include <lsm/structures/marshal.hpp>

#include <absl/container/btree_map.h>
#include <flat_map/flat_map.hpp>

namespace lsm::structures {

class SparseIndex {
public:
    using TMap = flat_map::flat_map<
        std::string,
        std::streamoff,
        std::less<>,
        std::vector<std::pair<std::string, std::streamoff>>>;

    SparseIndex(TMap index = {})
        : index_(std::move(index))
    { }

    std::streamoff getStartPos(const std::string& key) const;

    void push_back(std::string key, std::streamoff pos);

    const TMap index() const { return index_; }

private:
    TMap index_;
};

namespace marshal::impl {

template <>
SparseIndex fromStreamImpl<SparseIndex>(std::istream& istream);

template <>
std::streamoff toStreamImpl<SparseIndex>(const SparseIndex& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
