#include <lsm/structures/marshal.hpp>

#include <map>

namespace lsm::structures {

class SparseIndex {
public:
    SparseIndex(std::map<std::string, std::streamoff> index = {})
        : index_(std::move(index))
    {
        for (const auto& [k, v] : index) {
            // std::cerr << "sparseIndex: key: " << k << " offset: " << v << std::endl;
        }
    }

    std::streamoff getStartPos(const std::string& key) const;

    void emplace(std::string key, std::streamoff pos);

    const std::map<std::string, std::streamoff> index() const { return index_; }

private:
    std::map<std::string, std::streamoff> index_;
};

namespace marshal::impl {

template <>
SparseIndex fromStreamImpl<SparseIndex>(std::istream& istream);

template <>
std::streamoff toStreamImpl<SparseIndex>(const SparseIndex& value, std::ostream& ostream);

} // namespace marshal::impl

} // namespace lsm::structures
