/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_UPDATE__HPP_GUARD
#define SQLCPP_COMPONENTS_UPDATE__HPP_GUARD
#include "sqlcpp/components/assign.hpp"
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/components/expr.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/components/where.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <string>
#include <vector>
namespace sqlcpp {

    /// @brief 更新语句构建器
    struct Update final : public Builder {
        /// @brief 在update遇到约束冲突时的处理方式
        enum UpdateOr {
            OR_ROLLBACK,///< 回滚当前事务, 如果没有事务，则仅回滚当前语句
            OR_ABORT,   ///< 默认方式, 中止当前的操作，但不会回滚事务
            OR_FAIL,    ///< 类似于ABORT
            OR_IGNORE,  ///< 忽略错误并继续执行剩余的操作, 等于调用ignore, mysql支持
            OR_REPLACE, ///< 试图通过删除导致冲突的现有记录并插入新的记录来解决约束冲突
        };


        bool LOW_PRIORITY = false;                              ///< mysql only - 指定更新操作为低优先级，通常用于减少对其他查询的锁竞争
        bool IGNORE = false;                                    ///< mysql only - 在执行更新操作时忽略导致错误的行，而不会引发错误
        std::optional<UpdateOr> UPDATE_OR_{};                   ///< sqlite only - 在sqlite中控制约束冲突时的行为
        std::optional<Froms> table_{};                          ///< 要更新的表，指定更新操作的目标表
        std::optional<Assigns> assigns_{};                      ///< 要更新的列和值的集合
        std::optional<Where> where_{};                          ///< 条件子句，指定要更新的行
        std::optional<OrderBy> order_by_{};                     ///< 用于指定更新顺序的排序子句
        std::optional<std::variant<VarValue, size_t>> limit_{}; ///< 限制要更新的行数
        std::optional<std::variant<VarValue, size_t>> offset_{};///< sqlite only - 指定更新起始的行数偏移量
        std::optional<std::vector<FieldLike>> RETURNING_{};     ///< sqlite only - 指定返回的列值集合，类似于select语句的作用


        /// @brief 空构造
        Update() = default;
        /// @brief 指定赋值集合构造, 不指定表和条件
        /// @param assigns 赋值集合
        Update(Assigns assigns);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(const char *table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(std::string table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(From table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(RawFrom table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(Froms table, std::optional<Assigns> assigns = std::nullopt, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(const char *table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(std::string table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(From table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(RawFrom table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(Froms table, FieldLike field, ValueLike value = VAR, std::optional<Where> where = std::nullopt);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(const char *table, std::optional<Assigns> assigns, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(std::string table, std::optional<Assigns> assigns, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(From table, std::optional<Assigns> assigns, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(RawFrom table, std::optional<Assigns> assigns, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param assigns 要更新的列和值的集合
        /// @param where 条件子句
        Update(Froms table, std::optional<Assigns> assigns, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(const char *table, FieldLike field, ValueLike value, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(std::string table, FieldLike field, ValueLike value, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(From table, FieldLike field, ValueLike value, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(RawFrom table, FieldLike field, ValueLike value, Condition where);
        /// @brief 构造Update语句
        /// @param table 要更新的表
        /// @param field 要更新的列
        /// @param value 要更新的值
        /// @param where 条件子句
        Update(Froms table, FieldLike field, ValueLike value, Condition where);


        /// @brief 设置更新操作为低优先级
        /// @note mysql only
        Update &low_priority(bool v = true);
        /// @brief 设置在执行更新操作时忽略导致错误的行
        Update &ignore(bool v = true);
        /// @brief 设置sqlite中控制约束冲突时的行为
        /// @note sqlite only
        Update &update_or(std::optional<UpdateOr> v);

        /// @brief 设置要更新的表
        Update &table(std::string table);
        /// @brief 设置要更新的表
        Update &table(From table);
        /// @brief 设置要更新的表
        Update &table(RawFrom table);
        /// @brief 设置要更新的表
        Update &table(Froms tables);

        /// @brief 设置要更新的列和值
        Update &assign(Assign assign);
        /// @brief 设置要更新的列和值
        Update &assign(FieldLike field, ValueLike value = VAR);
        /// @brief 设置要更新的列和值的集合
        Update &assign(Assigns assigns);
        /// @brief 添加要更新的列和值
        Update &add_assign(Assign assign);
        /// @brief 添加要更新的列和值
        Update &add_assign(FieldLike field, ValueLike value = VAR);
        /// @brief 添加要更新的列和值的集合
        Update &add_assign(const Assigns &assigns);


        /// @brief 设置过滤条件
        Update &where(Where where);
        /// @brief 设置过滤条件
        Update &where(Condition where);
        /// @brief 设置排序条件
        Update &order_by(OrderBy order_by);
        /// @brief 设置排序条件
        Update &order_by(Field field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Update &order_by(RawField field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Update &order_by(ExprLike field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Update &order_by(FieldLike field, Order o = Order::ASC);
        /// @brief 设置排序条件
        Update &order_by(OrderByField field);
        /// @brief 设置限制条数
        Update &limit(size_t limit);
        /// @brief 设置偏移量
        /// @note sqlite only
        Update &offset(size_t offset);
        /// @brief 设置限制条数
        Update &limit(const VarValue &limit);
        /// @brief 设置偏移量
        /// @note sqlite only
        Update &offset(const VarValue &offset);


        /// @brief 设置返回的列
        /// @note sqlite only
        Update &returning(FieldLike r);
        /// @brief 设置返回的列
        /// @note sqlite only
        Update &returning(std::optional<std::vector<FieldLike>> r);
        /// @brief 设置返回的列
        /// @note sqlite only
        template<typename... Args>
        Update &returning(Args &&...rs);
        /// @brief 添加返回的列
        /// @note sqlite only
        Update &add_returning(FieldLike rs);
        /// @brief 添加返回的列
        /// @note sqlite only
        Update &add_returning(const std::vector<FieldLike> &rs);
        /// @brief 添加返回的列
        /// @note sqlite only
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
