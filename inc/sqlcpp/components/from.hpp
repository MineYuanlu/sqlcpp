/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FROM__HPP_GUARD
#define SQLCPP_COMPONENTS_FROM__HPP_GUARD
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/defs.hpp"
#include <string>
#include <variant>
namespace sqlcpp {
    /// @brief 表链接类型
    enum JoinType {
        /// @brief 内连接
        /// @details 只返回两个表中符合连接条件的匹配行
        /// @details 如果左表或右表中没有匹配的行，则不会返回
        INNER_JOIN,
        /// @brief 左连接
        /// @details 返回左表中所有行，右表中匹配的行
        /// @details 右表中没有匹配的行，会被填充为NULL
        LEFT_JOIN,
        /// @brief 右连接
        /// @details 返回右表中所有行，左表中匹配的行
        /// @details 左表中没有匹配的行，会被填充为NULL
        RIGHT_JOIN,
        /// @brief 全连接
        /// @details 返回两个表中所有行，匹配的行和不匹配的行
        /// @details 左表或右表中没有匹配的行，会被填充为NULL
        FULL_JOIN,
    };
    struct Froms;
    struct RawFrom;

    /// @brief 代表一个数据表引用
    struct From final : public Builder {
        std::string table_name_;            ///< 表名
        std::optional<std::string> alias_{};///< 别名

        /// @brief 使用表名构建
        From(const char *table);
        /// @brief 使用表名构建
        From(std::string table);
        /// @brief 使用表名和别名构建
        From(std::string table, std::optional<std::string> alias);
        /// @brief 设置别名
        From &alias(std::string alias);

        /// @brief 获取此表内的字段
        Field operator[](std::string field) const;
        /// @brief 连接另一个表
        Froms join(JoinType t, From from, Condition cond) const;
        /// @brief 连接另一个表
        Froms join(JoinType t, RawFrom from, Condition cond) const;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    /// @brief 原生数据表引用
    /// @note 注意数据安全
    struct RawFrom final : public Builder {
        std::string raw_from_;

        explicit RawFrom(std::string from);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    /// @brief 连接类型字符串
    constexpr inline const char *join_type_str(JoinType type) {
        // clang-format off
        switch (type) {
            case INNER_JOIN: return "INNER JOIN";
            case LEFT_JOIN: return "LEFT JOIN";
            case RIGHT_JOIN: return "RIGHT JOIN";
            case FULL_JOIN: return "FULL OUTER JOIN";
        }
        // clang-format on
        return "";
    }


    /// @brief 多个表的连接
    struct Froms final : public Builder {
        std::variant<From, RawFrom> from_;                                                 ///< 主表
        std::vector<std::tuple<JoinType, std::variant<From, RawFrom>, Condition>> joins_{};///< 连接表及条件

        Froms(From);
        Froms(RawFrom);
        /// @brief 连接另一个表
        Froms &join(JoinType t, From from, Condition cond);
        /// @brief 连接另一个表
        Froms &join(JoinType t, RawFrom from, Condition cond);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_FROM__HPP_GUARD
