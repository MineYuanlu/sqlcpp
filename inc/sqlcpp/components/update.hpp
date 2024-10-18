/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_UPDATE__HPP_GUARD
#define SQLCPP_COMPONENTS_UPDATE__HPP_GUARD
#include "sqlcpp/components/assign.hpp"
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/components/where.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <string>
#include <vector>
namespace sqlcpp {
    struct Update final : public Builder {
        enum UpdateOr {
            OR_ROLLBACK,
            OR_ABORT,
            OR_FAIL,
            OR_IGNORE,///< 等于调用ignore, mysql支持
            OR_REPLACE,
        };


        bool LOW_PRIORITY = false;           ///< mysql only
        bool IGNORE = false;                 ///< mysql only
        std::optional<UpdateOr> UPDATE_OR_{};///< sqlite only
        std::optional<Froms> table_{};
        std::optional<Assigns> assigns_{};
        std::optional<Where> where_{};
        std::optional<OrderBy> order_by_{};
        std::optional<std::variant<VarValue, size_t>> limit_{};
        std::optional<std::variant<VarValue, size_t>> offset_{};///< sqlite only
        std::optional<std::vector<FieldLike>> RETURNING_{};     ///< sqlite only

        Update() = default;
        Update(Assigns assigns);
        Update(const char *table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        Update(std::string table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        Update(From table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        Update(RawFrom table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        Update(Froms table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        Update(const char *table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        Update(std::string table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        Update(From table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        Update(RawFrom table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        Update(Froms table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        Update(const char *table, std::optional<Assigns> assigns, Condition where);
        Update(std::string table, std::optional<Assigns> assigns, Condition where);
        Update(From table, std::optional<Assigns> assigns, Condition where);
        Update(RawFrom table, std::optional<Assigns> assigns, Condition where);
        Update(Froms table, std::optional<Assigns> assigns, Condition where);
        Update(const char *table, FieldLike field, ValueLike value, Condition where);
        Update(std::string table, FieldLike field, ValueLike value, Condition where);
        Update(From table, FieldLike field, ValueLike value, Condition where);
        Update(RawFrom table, FieldLike field, ValueLike value, Condition where);
        Update(Froms table, FieldLike field, ValueLike value, Condition where);


        Update &low_priority(bool v = true);
        Update &ignore(bool v = true);
        Update &update_or(std::optional<UpdateOr> v);

        Update &table(std::string table);
        Update &table(From table);
        Update &table(RawFrom table);
        Update &table(Froms tables);

        Update &assign(Assign assign);
        Update &assign(FieldLike field, ValueLike value = VAR);
        Update &assign(Assigns assigns);
        Update &add_assign(Assign assign);
        Update &add_assign(FieldLike field, ValueLike value = VAR);
        Update &add_assign(const Assigns &assigns);


        Update &where(Where where);
        Update &where(Condition where);
        Update &order_by(OrderBy order_by);
        Update &order_by(Field field, Order o = Order::ASC);
        Update &order_by(RawField field, Order o = Order::ASC);
        Update &order_by(FuncField field, Order o = Order::ASC);
        Update &order_by(FieldLike field, Order o = Order::ASC);
        Update &order_by(OrderByField field);
        Update &limit(size_t limit);
        ///@note sqlite only
        Update &offset(size_t offset);
        Update &limit(const VarValue &limit);
        ///@note sqlite only
        Update &offset(const VarValue &offset);


        ///@note sqlite only
        Update &returning(FieldLike r);
        ///@note sqlite only
        Update &returning(std::optional<std::vector<FieldLike>> r);
        ///@note sqlite only
        template<typename... Args>
        Update &returning(Args &&...rs);
        ///@note sqlite only
        Update &add_returning(FieldLike rs);
        ///@note sqlite only
        Update &add_returning(const std::vector<FieldLike> &rs);
        ///@note sqlite only
        template<typename... Args>
        Update &add_returning(Args &&...rs);


        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    template<typename... Args>
    Update &Update::returning(Args &&...rs) {
        RETURNING_.emplace();
        RETURNING_->reserve(sizeof...(Args));
        (RETURNING_->emplace_back(std::forward<Args>(rs)), ...);
        return *this;
    }
    template<typename... Args>
    Update &Update::add_returning(Args &&...rs) {
        if (!RETURNING_) RETURNING_ = std::vector<FieldLike>{};
        RETURNING_->reserve(RETURNING_->size() + sizeof...(Args));
        (RETURNING_->emplace_back(std::forward<Args>(rs)), ...);
        return *this;
    }

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_UPDATE__HPP_GUARD
