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


    /// @brief 排序顺序
    enum Order {
        ASC, ///< 升序排列, 默认顺序. 数字从小到大, 字母从 A 到 Z
        DESC,///< 降序排列, 数字从大到小, 字母从 Z 到 A
    };
    /// @brief 字符串化排序顺序
    constexpr inline const char *order_str(Order order) {
        return order == ASC ? "ASC" : "DESC";
    }


    /// @brief 单个字段的排序条件
    struct OrderByField final : public Builder, public VarBuilder {
        FieldLike field_;             ///< 排序字段
        Order direction_ = Order::ASC;///< 排序顺序

        OrderByField(FieldLike field, Order o = Order::ASC);
        OrderByField(Field field, Order o = Order::ASC);
        OrderByField(RawField field, Order o = Order::ASC);
        OrderByField(Expr field, Order o = Order::ASC);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


    /// @brief 多个字段的排序条件
    struct OrderBy final : public Builder, public VarBuilder {
        std::vector<OrderByField> fields_;///< 排序字段列表

        OrderBy(Field field, Order o = Order::ASC);
        OrderBy(RawField field, Order o = Order::ASC);
        OrderBy(Expr field, Order o = Order::ASC);
        OrderBy(FieldLike field, Order o = Order::ASC);
        OrderBy(OrderByField field);
        OrderBy(std::vector<OrderByField> fields);
        template<typename... Args>
        OrderBy(OrderByField field, Args &&...args) : fields_({std::move(field)}) {
            by(std::forward<Args>(args)...);
        }
        OrderBy &by(Field field, Order o = Order::ASC);
        OrderBy &by(RawField field, Order o = Order::ASC);
        OrderBy &by(Expr field, Order o = Order::ASC);
        OrderBy &by(FieldLike field, Order o = Order::ASC);
        OrderBy &by(OrderByField field);
        template<typename... Args>
        inline OrderBy &by(Args &&...args) {
            (fields_.emplace_back(std::forward<Args>(args)), ...);
            return *this;
        }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_ORDER_BY__HPP_GUARD
