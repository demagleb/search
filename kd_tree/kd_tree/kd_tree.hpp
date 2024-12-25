#include <functional>
#include <vector>
#include <memory>
#include <span>

namespace kd_tree {

class KDTree {
public:
    using Vector = std::vector<double>;
    using VectorCRef = std::reference_wrapper<const Vector>;

    KDTree(std::vector<Vector> vectors, size_t leafSize = 5);
    ~KDTree();

    bool contains(const Vector& vec) const;
    bool insert(Vector vec);

    const Vector& findNearest(const Vector& vec) const;
    std::vector<KDTree::VectorCRef> findKNearest(const Vector& vec, size_t k) const;

private:
    struct Node;
    std::unique_ptr<Node> build(std::span<Vector> data, size_t level = 0);
    Node& findNode(const Vector& vec) const;

    size_t leafSize_ = 0;
    std::unique_ptr<Node> root_;
};

} // namespace kd_tree
