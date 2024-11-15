/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_DELETE__HPP_GUARD
#define SQLCPP_COMPONENTS_DELETE__HPP_GUARD
#include "sqlcpp/components/expr.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/components/where.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
namespace sqlcpp {
    /// @brief 删除语句构建器
    struct Delete final : public Builder {
        bool LOW_PRIORITY = false;///< mysql only
        bool QUICK = false;       ///< mysql only
        bool IGNORE = false;      ///< mysql only
        std::optional<Froms> table_;
        std::optional<Where> where_;
        std::optional<OrderBy> order_by_;
        std::optional<std::variant<VarValue, size_t>> limit_;
        std::optional<std::variant<VarValue, size_t>> offset_;///< sqlite only

        Delete() = default;
        Delete(const Delete &) = default;
        Delete(Delete &&) = default;
        Delete &operator=(const Delete &) = default;
        Delete &operator=(Delete &&) = default;
        Delete(const char *table, std::optional<Where> where = std::nullopt);
        Delete(std::string table, std::optional<Where> where = std::nullopt);
        Delete(From table, std::optional<Where> where = std::nullopt);
        Delete(RawFrom table, std::optional<Where> where = std::nullopt);
        Delete(Froms table, std::optional<Where> where = std::nullopt);
        Delete(const char *table, Condition where);
        Delete(std::string table, Condition where);
        Delete(From table, Condition where);
        Delete(RawFrom table, Condition where);
        Delete(Froms table, Condition where);

        Delete &low_priority(bool v = true);
        Delete &quick(bool v = true);
        Delete &ignore(bool v = true);

        Delete &from(const char *table);
        Delete &from(std::string table);
        Delete &from(From table);
        Delete &from(RawFrom table);
        Delete &from(Froms table);

        Delete &where(Condition where);
        Delete &where(Where where);

        Delete &order_by(OrderBy order_by);
        Delete &order_by(Field field, Order o = Order::ASC);
        Delete &order_by(RawField field, Order o = Order::ASC);
        Delete &order_by(ExprLike field, Order o = Order::ASC);
        Delete &order_by(FieldLike field, Order o = Order::ASC);
        Delete &order_by(OrderByField field);

        Delete &limit(size_t limit);
        ///@note sqlite only
        Delete &offset(size_t offset);
        Delete &limit(const VarValue &limit);
        ///@note sqlite only
        Delete &offset(const VarValue &offset);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_DELETE__HPP_GUARD
