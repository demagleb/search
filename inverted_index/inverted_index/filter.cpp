#include <inverted_index/filter.hpp>
#include <memory>

namespace inverted_index {

namespace {

class HaveFilter : public IFilter{
public:
    HaveFilter(std::string term)
        : term_(std::move(term))
    { }

    ~HaveFilter() = default;

    roaring::Roaring64Map get(InvertedIndex& index) override final {
        return index.getByWord(term_);
    };

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<HaveFilter>(term_);
    }

private:
    std::string term_;
};

class BinaryFilter : public IFilter {
public:
    enum class Type {
        And,
        Or,
        Minus
    };

    BinaryFilter(
        std::unique_ptr<IFilter> lhs,
        std::unique_ptr<IFilter> rhs,
        Type type)
        : lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
        , type_(type)
    { }

    roaring::Roaring64Map get(InvertedIndex& index) {
        switch (type_) {
            case Type::And : {
                return lhs_->get(index) & rhs_->get(index);
            }
            case Type::Or : {
                return lhs_->get(index) | rhs_->get(index);
            }
            case Type::Minus : {
                return lhs_->get(index) - rhs_->get(index);
            }
        }
        return {};
    };

    std::unique_ptr<IFilter> copy() {
        return std::make_unique<BinaryFilter>(
            lhs_->copy(),
            rhs_->copy(),
            type_);
    }

private:
    std::unique_ptr<IFilter> lhs_;
    std::unique_ptr<IFilter> rhs_;
    Type type_;
};

} // namespace


Filter::Filter(const Filter& other)
    : filter_(other.filter_->copy())
{ }

Filter::Filter(Filter&& other)
    : filter_(std::move(other.filter_))
{ }

Filter& Filter::operator=(const Filter& other) {
    filter_ = other.filter_->copy();
    return *this;
}

Filter& Filter::operator=(Filter&& other) {
    filter_ = std::move(other.filter_);
    return *this;
}

Filter Filter::have(std::string term) {
    return Filter(std::make_unique<HaveFilter>(std::move(term)));
}

Filter& Filter::operator&=(const Filter& other) {
    filter_ = std::make_unique<BinaryFilter>(
        std::move(filter_),
        other.filter_->copy(),
        BinaryFilter::Type::And);
    return *this;
}

Filter& Filter::operator|=(const Filter& other) {
    filter_ = std::make_unique<BinaryFilter>(
        std::move(filter_),
        other.filter_->copy(),
        BinaryFilter::Type::Or);
    return *this;
}
Filter& Filter::operator-=(const Filter& other) {
    filter_ = std::make_unique<BinaryFilter>(
        std::move(filter_),
        other.filter_->copy(),
        BinaryFilter::Type::Minus);
    return *this;
}

Filter Filter::operator&(const Filter& other) {
    return Filter(std::make_unique<BinaryFilter>(
        copy(),
        other.filter_->copy(),
        BinaryFilter::Type::And));
}

Filter Filter::operator|(const Filter& other) {
    return Filter(std::make_unique<BinaryFilter>(
        copy(),
        other.filter_->copy(),
        BinaryFilter::Type::Or));
}
Filter Filter::operator-(const Filter& other) {
    return Filter(std::make_unique<BinaryFilter>(
        copy(),
        other.filter_->copy(),
        BinaryFilter::Type::Minus));
}

} // namespace inverted_index
