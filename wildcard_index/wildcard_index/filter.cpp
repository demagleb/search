#include "bitmap_index/filter.hpp"
#include "wildcard_index/wildcard_index.hpp"
#include <memory>
#include <stdexcept>
#include <wildcard_index/filter.hpp>

namespace wildcard_index::filter {

namespace {

class HaveWildcardFilter : public IFilter {
public:
    HaveWildcardFilter(std::string wildcard)
        : wildcard_(std::move(wildcard))
    { }

    ~HaveWildcardFilter() = default;

    roaring::Roaring64Map exec(bitmap_index::BitmapIndex& index) override final {
        wildcard_index::WildcardIndex* wildcardIndex = dynamic_cast<wildcard_index::WildcardIndex*>(&index);

        if (wildcardIndex == nullptr) {
            throw std::runtime_error("Can't cast index to WildcardIndex");
        }

        return wildcardIndex->getByWildcard(wildcard_);
    }

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<HaveWildcardFilter>(wildcard_);
    }

private:
    std::string wildcard_;
};

} // namespace

Filter haveWildcard(std::string wildcard) {
    return Filter(std::make_unique<HaveWildcardFilter>(std::move(wildcard)));
}

} // namespace wildcard_index::filter
