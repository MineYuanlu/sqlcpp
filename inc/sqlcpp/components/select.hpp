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
#include "sqlcpp/components/value.hpp"
#include "sqlcpp/components/where.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <vector>
namespace sqlcpp {

    /// @brief 选择语句构建器
    struct Select final : public Builder, public VarBuilder {
        std::vector<FieldLike> fields_{};  ///< 选择的字段
        std::optional<Froms> from_{};      ///< 表名(和join关系)
        std::optional<Where> where_{};     ///< 过滤条件
        std::optional<GroupBy> group_by_{};///< 分组条件
        std::optional<Having> having_{};   ///< 分组后的过滤条件
        std::optional<OrderBy> order_by_{};///< 排序条件

        std::optional<std::variant<VarValue, IndexedVarValue, size_t>> limit_{}; ///< 限制条数
        std::optional<std::variant<VarValue, IndexedVarValue, size_t>> offset_{};///< 偏移量

        Select(Select &&) = default;
        Select &operator=(Select &&) = default;
        Select(const Select &) = default;
        Select &operator=(const Select &) = default;


        /// @brief 空构造函数
        explicit Select() = default;
        /// @brief 单字段构造函数
        Select(FieldLike field);
        /// @brief 多字段构造函数
        Select(std::vector<FieldLike> fields);
        /// @brief 多字段构造函数
        template<
                typename T,
                typename... Args,
                typename = std::enable_if_t<
                        (std::is_convertible_v<std::decay_t<T>, Field> || std::is_convertible_v<std::decay_t<T>, RawField> || std::is_convertible_v<std::decay_t<T>, Expr>) &&(sizeof...(Args) > 0)>>
        Select(T field, Args &&...args) { select(field, std::forward<Args>(args)...); }

        /// @brief 选择字段
        Select &select(const char *field);
        /// @brief 选择字段
        Select &select(std::string field);
        /// @brief 选择字段
        Select &select(FieldLike field);
        /// @brief 选择字段
        Select &select(Field field);
        /// @brief 选择字段
        Select &select(RawField field);
        /// @brief 选择字段
        Select &select(Expr field);
        /// @brief 选择字段
        template<
                typename T,
                typename... Args,
                typename = std::enable_if_t<
                        (std::is_convertible_v<std::decay_t<T>, Field> || std::is_convertible_v<std::decay_t<T>, RawField> || std::is_convertible_v<std::decay_t<T>, Expr>) &&(sizeof...(Args) > 0)>>
        inline Select &select(T field, Args &&...args);

        /// @brief 设置完整的from关系
        Select &from(Froms from);
        /// @brief 设置单表from
        Select &from(From from);
        /// @brief 设置from
        Select &from(RawFrom from);

        /// @brief 设置过滤条件
        Select &where(Where where);
        /// @brief 设置过滤条件
        Select &where(Condition where);
        /// @brief 设置分组条件
        Select &group_by(GroupBy group_by);
        /// @brief 设置分组过滤条件
        Select &having(Having having);
        /// @brief 设置分组过滤条件
        Select &having(Condition having);
        /// @brief 设置排序条件
        Select &order_by(OrderBy order_by);
        /// @brief 设置排序条件
        Select &order_by(Field field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Select &order_by(RawField field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Select &order_by(Expr field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Select &order_by(FieldLike field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Select &order_by(OrderByField field);
        /// @brief 设置限制条数
        Select &limit(size_t limit);
        /// @brief 设置偏移量
        Select &offset(size_t offset);
        /// @brief 设置限制条数
        Select &limit(const VarValue &limit);
        /// @brief 设置偏移量
        Select &offset(const VarValue &offset);
        /// @brief 设置限制条数
        Select &limit(const IndexedVarValue &limit);
        /// @brief 设置偏移量
        Select &offset(const IndexedVarValue &offset);


        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void build_s(std::ostream &oss, const Type &t, bool is_subquery) const;
        void edit_var_map(VarMap &var_map) const override;
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
