#include <vector>
#include <optional>

namespace kd_tree {

class BBox {
public:
    void add(const std::vector<double>& vec);
    void add(const BBox& bbox);
    double distance2(const std::vector<double>& vec) const;

private:
    std::vector<double> l_;
    std::vector<double> r_;
};

} // namespace kd_tree
