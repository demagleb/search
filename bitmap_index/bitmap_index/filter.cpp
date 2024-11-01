#include "roaring64map.hh"
#include <bitmap_index/filter.hpp>
#include <memory>

namespace bitmap_index::filter {

namespace {

class HaveFilter : public IFilter {
public:
    HaveFilter(std::string term)
        : term_(std::move(term))
    { }

    ~HaveFilter() = default;

    roaring::Roaring64Map exec(BitmapIndex& index) override final {
        return index.getByKey(term_);
    };

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<HaveFilter>(term_);
    }

private:
    std::string term_;
};

class BetweenFilter : public IFilter {
public:
    BetweenFilter(std::string left, std::string right)
        : left_(std::move(left))
        , right_(std::move(right))
    { }

    ~BetweenFilter() = default;

    roaring::Roaring64Map exec(BitmapIndex& index) override final {
        roaring::Roaring64Map bitmap;
        for (auto& value : index.getByKeyRange(left_, right_)) {
            bitmap |= value;
        }
        return bitmap;
    };

    std::unique_ptr<IFilter> copy() override final {
        return std::make_unique<BetweenFilter>(left_, right_);
    }

private:
    std::string left_;
    std::string right_;
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

    roaring::Roaring64Map exec(BitmapIndex& index) {
        switch (type_) {
            case Type::And : {
                return lhs_->exec(index) & rhs_->exec(index);
            }
            case Type::Or : {
                return lhs_->exec(index) | rhs_->exec(index);
            }
            case Type::Minus : {
                return lhs_->exec(index) - rhs_->exec(index);
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

Filter::Filter(std::unique_ptr<IFilter> filter)
    : filter_(std::move(filter))
{ }

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

Filter have(std::string key) {
    return Filter(std::make_unique<HaveFilter>(std::move(key)));
}

Filter between(std::string left, std::string right) {
    return Filter(std::make_unique<BetweenFilter>(std::move(left), std::move(right)));
}

} // namespace bitmap_index::filter
