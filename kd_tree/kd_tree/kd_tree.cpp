#include "fmt/base.h"
#include <algorithm>
#include <compare>
#include <functional>
#include <kd_tree/kd_tree.hpp>
#include <kd_tree/bbox.hpp>
#include <memory>
#include <queue>
#include <set>
#include <algorithm>
#include <cassert>


#include <fmt/format.h>

namespace kd_tree {

struct KDTree::Node {
    std::unique_ptr<KDTree::Node> left;
    std::unique_ptr<KDTree::Node> right;

    BBox bbox;

    double median;
    std::vector<KDTree::Vector> data;
    size_t level;
};

KDTree::~KDTree() = default;

KDTree::KDTree(std::vector<Vector> vectors, size_t leafSize)
    : leafSize_(leafSize)
    , root_(build(vectors))
{ }


std::unique_ptr<KDTree::Node> KDTree::build(std::span<Vector> data, size_t level)
{
    auto node = std::make_unique<Node>();
    node->level = level;

    if (data.size() < 2 * leafSize_) {
        for (const auto& vec : data) {
            node->bbox.add(vec);
        }
        node->data.reserve(data.size());
        for (auto& x : data) {
            node->data.push_back(std::move(x));
        }
        return node;
    }
    const size_t median = data.size() / 2;
    const size_t idx = level % data.front().size();
    std::nth_element(data.begin(), data.begin() + median, data.end(), [&](const auto& lhs, const auto& rhs) {
        return lhs.at(idx) < rhs.at(idx);
    });
    node->median = data[median][idx];

    size_t nl = std::partition(data.begin(), data.end(), [&](const auto& v) {
        return v[idx] <= node->median;
    }) - data.begin();

    node->left = build(data.subspan(0, nl), level + 1);
    node->right = build(data.subspan(nl), level + 1);
    node->bbox.add(node->left->bbox);
    node->bbox.add(node->right->bbox);
    return node;
}

KDTree::Node& KDTree::findNode(const Vector& vec) const
{
    Node* node = root_.get();
    while (node->left != nullptr) {
        size_t idx = node->level % vec.size();
        if (vec[idx] <= node->median) {
            node = node->left.get();
        } else {
            node = node->right.get();
        }
    }
    return *node;
}

std::vector<KDTree::VectorCRef> KDTree::findKNearest(const Vector& target, size_t k) const
{
    if (k == 0) {
        return {};
    }

    struct NodeWithDist {
        const Node* node;
        double dist;

        std::partial_ordering operator<=> (const NodeWithDist& other) const {
            return dist <=> other.dist;
        }
    };

    struct VectorWithDist {
        const Vector* vec;
        double dist;
        std::partial_ordering operator<=> (const VectorWithDist& other) const {
            return dist <=> other.dist;
        }
    };

    auto distance2 = [](const auto& lhs, const auto& rhs) {
        double sum  = 0;
        for (size_t i = 0; i < lhs.size(); ++i) {
            sum += (lhs.at(i) - rhs.at(i)) * (lhs.at(i) - rhs.at(i));
        }
        return sum;
    };

    std::set<VectorWithDist> best;

    std::priority_queue<NodeWithDist, std::vector<NodeWithDist>, std::greater<NodeWithDist>> queue;
    queue.push(NodeWithDist{.node = root_.get(), .dist = root_.get()->bbox.distance2(target)});
    while (!queue.empty()) {
        const Node& node = *queue.top().node;
        double dist = queue.top().dist;
        queue.pop();

        if (best.size() == k && dist > (--best.end())->dist) {
            continue;
        }

        if (node.left == nullptr) {
            for (const auto& vec : node.data) {
                best.insert(VectorWithDist{.vec = &vec, .dist = distance2(vec, target)});
                if (best.size() > k) {
                    best.erase(--best.end());
                }
            }
            continue;
        }

        queue.push(NodeWithDist{.node = node.left.get(), .dist = node.left->bbox.distance2(target)});
        queue.push(NodeWithDist{.node = node.right.get(), .dist = node.right->bbox.distance2(target)});
    }

    std::vector<VectorCRef> result;
    result.reserve(best.size());
    for (const auto x : best) {
        result.push_back(*x.vec);
    }
    return result;
}

const KDTree::Vector& KDTree::findNearest(const Vector& vec) const
{
    return findKNearest(vec, 1).front().get();
}

bool KDTree::contains(const Vector& vec) const {
    Node& node = findNode(vec);
    return std::count(node.data.begin(), node.data.end(), vec);
}

bool KDTree::insert(Vector vec) {
    Node& node = findNode(vec);
    if (!std::count(node.data.begin(), node.data.end(), vec)) {
        node.data.push_back(std::move(vec));
        return true;
    }
    return false;
}

} // namespace kd_tree
