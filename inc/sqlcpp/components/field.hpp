/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FIELD__HPP_GUARD
#define SQLCPP_COMPONENTS_FIELD__HPP_GUARD


#include "sqlcpp/components/expr.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <variant>
#include <vector>
namespace sqlcpp {

    struct OrderByField;
    struct ValueLike;
    struct CondCmp;
    struct CondIn;
    struct CondNotIn;
    struct CondBetween;
    struct Assign;

    /// @brief 一个普通字段, 用于查询/更新/删除/条件等操作
    /// @details 可以指定表、别称
    struct Field final : public Builder, public VarBuilder {
        std::optional<std::string> table_name_{};///< 表名
        std::string field_name_;                 ///< 字段名
        std::optional<std::string> alias_{};     ///< 别称
        bool distinct_{false};                   ///< 是否为distinct查询

        /// @brief 使用字段名构造
        Field(const char *field);
        /// @brief 使用字段名构造
        Field(std::string field);
        /// @brief 使用表名和字段名构造
        Field(std::string table, std::string field);
        /// @brief 使用表名, 字段名和别称构造
        Field(std::string table, std::string field, std::string alias);
        /// @brief 设置表名
        Field &table(std::string table);
        /// @brief 设置别称
        Field &alias(std::string alias);
        /// @brief 设置别称
        Field &as(std::string alias);
        /// @brief 设置是否为distinct查询
        Field &distinct(bool v = true);

        /// @brief 返回此字段的升序排序语句
        OrderByField asc() const;
        /// @brief 返回此字段的降序排序语句
        OrderByField desc() const;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;

        /// @brief 返回like比较条件语句
        CondCmp LIKE(ValueLike v) const;
        /// @brief 返回not like比较条件语句
        CondCmp NOT_LIKE(ValueLike v) const;
        /// @brief 返回in比较条件语句
        CondIn IN(std::vector<ValueLike> vs) const;
        /// @brief 返回not in比较条件语句
        CondNotIn NOT_IN(std::vector<ValueLike> vs) const;
        /// @brief 返回between比较条件语句
        CondBetween BETWEEN(ValueLike start, ValueLike end) const;

        /// @brief 返回此字段的赋值语句
        Assign assign(ValueLike) const;
        /// @brief 返回此字段的赋值语句
        Assign operator=(ValueLike) const;
    };


    /// @brief 原生字段, 不会进行任何转义, 用于查询/更新/删除/条件等操作
    /// @note 注意数据安全
    struct RawField final : public Builder, public VarBuilder {
        std::string raw_field_;
        explicit RawField(std::string field);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


    /// @brief 形如字段的表达式, 用于查询/更新/删除/条件等操作, 可以是 Field / RawField / Expr
    struct FieldLike final : public Builder, public VarBuilder {
        std::variant<Field, RawField, Expr> field_;///< 实际字段

        /// @brief 使用字段名构造
        /// @see Field
        FieldLike(const char *field);
        /// @brief 使用字段名构造
        /// @see Field
        FieldLike(std::string field);
        /// @brief 使用表名和字段名构造
        /// @see Field
        FieldLike(std::string table, std::string field);
        /// @brief 使用表名, 字段名和别称构造
        /// @see Field
        FieldLike(std::string table, std::string field, std::string alias);
        /// @brief 使用字段构造
        FieldLike(Field field);
        /// @brief 使用字段构造
        FieldLike(RawField field);
        /// @brief 使用字段构造
        FieldLike(Expr field);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;

        /// @brief 返回like比较条件语句
        CondCmp LIKE(ValueLike v) const;
        /// @brief 返回not like比较条件语句
        CondCmp NOT_LIKE(ValueLike v) const;
        /// @brief 返回in比较条件语句
        CondIn IN(std::vector<ValueLike> vs) const;
        /// @brief 返回not in比较条件语句
        CondNotIn NOT_IN(std::vector<ValueLike> vs) const;
        /// @brief 返回between比较条件语句
        CondBetween BETWEEN(ValueLike start, ValueLike end) const;

        /// @brief 返回此字段的赋值语句
        Assign assign(ValueLike) const;
        /// @brief 返回此字段的赋值语句
        Assign operator=(ValueLike) const;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_FIELD__HPP_GUARD
