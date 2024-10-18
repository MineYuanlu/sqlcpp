/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_SELECT__HPP_GUARD
#define SQLCPP_COMPONENTS_SELECT__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/group_by.hpp"
#include "sqlcpp/components/having.hpp"
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/components/where.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <vector>
namespace sqlcpp {

    struct Select final : public Builder {
        std::vector<FieldLike> fields_;
        std::optional<Froms> from_;
        std::optional<Where> where_;
        std::optional<GroupBy> group_by_;
        std::optional<Having> having_;
        std::optional<OrderBy> order_by_;
        std::optional<std::variant<VarValue, size_t>> limit_;
        std::optional<std::variant<VarValue, size_t>> offset_;

        Select(Select &&) = default;
        Select &operator=(Select &&) = default;
        Select(const Select &) = default;
        Select &operator=(const Select &) = default;

        explicit Select() = default;
        Select(FieldLike field);
        Select(std::vector<FieldLike> fields);
        template<
                typename T,
                typename... Args,
                typename = std::enable_if_t<
                        (std::is_convertible_v<std::decay_t<T>, Field> || std::is_convertible_v<std::decay_t<T>, RawField> || std::is_convertible_v<std::decay_t<T>, FuncField>) &&(sizeof...(Args) > 0)>>
        Select(T field, Args &&...args) { select(field, std::forward<Args>(args)...); }
        Select &select(FieldLike field);
        Select &select(Field field);
        Select &select(RawField field);
        Select &select(FuncField field);
        template<
                typename T,
                typename... Args,
                typename = std::enable_if_t<
                        (std::is_convertible_v<std::decay_t<T>, Field> || std::is_convertible_v<std::decay_t<T>, RawField> || std::is_convertible_v<std::decay_t<T>, FuncField>) &&(sizeof...(Args) > 0)>>
        inline Select &select(T field, Args &&...args);

        Select &from(Froms from);
        Select &from(From from);
        Select &from(RawFrom from);

        Select &where(Where where);
        Select &where(Condition where);
        Select &group_by(GroupBy group_by);
        Select &having(Having having);
        Select &having(Condition having);
        Select &order_by(OrderBy order_by);
        Select &order_by(Field field, Order o = Order::ASC);
        Select &order_by(RawField field, Order o = Order::ASC);
        Select &order_by(FuncField field, Order o = Order::ASC);
        Select &order_by(FieldLike field, Order o = Order::ASC);
        Select &order_by(OrderByField field);
        Select &limit(size_t limit);
        Select &offset(size_t offset);
        Select &limit(const VarValue &limit);
        Select &offset(const VarValue &offset);


        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    template<
            typename T,
            typename... Args,
            typename>
    inline Select &Select::select(T field, Args &&...args) {
        select(std::move(field));
        return select(std::forward<Args>(args)...);
    }

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_SELECT__HPP_GUARD
