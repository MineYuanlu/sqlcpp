#include "sqlcpp/components/order_by.hpp"
namespace sqlcpp {
    OrderByField::OrderByField(FieldLike field, Order o) : field_(field), direction_(o) {}
    OrderByField::OrderByField(Field field, Order o) : field_(field), direction_(o) {}
    OrderByField::OrderByField(RawField field, Order o) : field_(field), direction_(o) {}
    OrderByField::OrderByField(FuncField field, Order o) : field_(field), direction_(o) {}

    void OrderByField::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        if (direction_ == Order::ASC)
            oss << " ASC";
        else
            oss << " DESC";
    }

    OrderBy::OrderBy(Field field, Order o) : OrderBy(OrderByField{std::move(field), o}) {}
    OrderBy::OrderBy(RawField field, Order o) : OrderBy(OrderByField{std::move(field), o}) {}
    OrderBy::OrderBy(FuncField field, Order o) : OrderBy(OrderByField{std::move(field), o}) {}
    OrderBy::OrderBy(FieldLike field, Order o) : OrderBy(OrderByField{std::move(field), o}) {}
    OrderBy::OrderBy(OrderByField field) : fields_({std::move(field)}) {}
    OrderBy::OrderBy(std::vector<OrderByField> fields) : fields_(std::move(fields)) {}

    OrderBy &OrderBy::by(Field field, Order o) {
        fields_.emplace_back(std::move(field), o);
        return *this;
    }
    OrderBy &OrderBy::by(RawField field, Order o) {
        fields_.emplace_back(std::move(field), o);
        return *this;
    }
    OrderBy &OrderBy::by(FuncField field, Order o) {
        fields_.emplace_back(std::move(field), o);
        return *this;
    }
    OrderBy &OrderBy::by(FieldLike field, Order o) {
        fields_.emplace_back(std::move(field), o);
        return *this;
    }
    OrderBy &OrderBy::by(OrderByField field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }

    void OrderBy::build_s(std::ostream &oss, const Type &t) const {
        if (fields_.empty()) return;
        oss << " ORDER BY ";
        const auto sz = fields_.size();
        for (size_t i = 0; i < sz; ++i) {
            fields_[i].build_s(oss, t);
            if (i < sz - 1) oss << ", ";
        }
    }

}// namespace sqlcpp
