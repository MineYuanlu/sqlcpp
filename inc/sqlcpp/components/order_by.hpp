/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_ORDER_BY__HPP_GUARD
#define SQLCPP_COMPONENTS_ORDER_BY__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/defs.hpp"
#include <vector>
namespace sqlcpp {


    enum Order {
        ASC,
        DESC,
    };
    constexpr inline const char *order_str(Order order) {
        return order == ASC ? "ASC" : "DESC";
    }


    struct OrderByField final : Builder {
        FieldLike field_;
        Order direction_ = Order::ASC;

        OrderByField(FieldLike field, Order o = Order::ASC);
        OrderByField(Field field, Order o = Order::ASC);
        OrderByField(RawField field, Order o = Order::ASC);
        OrderByField(FuncField field, Order o = Order::ASC);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    struct OrderBy final : Builder {
        std::vector<OrderByField> fields_;

        OrderBy(Field field, Order o = Order::ASC);
        OrderBy(RawField field, Order o = Order::ASC);
        OrderBy(FuncField field, Order o = Order::ASC);
        OrderBy(FieldLike field, Order o = Order::ASC);
        OrderBy(OrderByField field);
        OrderBy(std::vector<OrderByField> fields);
        template<typename... Args>
        OrderBy(OrderByField field, Args &&...args) : fields_({std::move(field)}) {
            by(std::forward<Args>(args)...);
        }
        OrderBy &by(Field field, Order o = Order::ASC);
        OrderBy &by(RawField field, Order o = Order::ASC);
        OrderBy &by(FuncField field, Order o = Order::ASC);
        OrderBy &by(FieldLike field, Order o = Order::ASC);
        OrderBy &by(OrderByField field);
        template<typename... Args>
        inline OrderBy &by(Args &&...args) {
            (fields_.emplace_back(std::forward<Args>(args)), ...);
            return *this;
        }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_ORDER_BY__HPP_GUARD
