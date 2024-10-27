/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_INDEX__HPP_GUARD
#define SQLCPP_COMPONENTS_INDEX__HPP_GUARD
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <vector>
namespace sqlcpp {

    /// @brief 索引组件, 创建字段一般的(普通/唯一)索引
    struct Index final : public Builder {
        std::optional<std::string> table_{};                  ///< 表名
        std::string name_{};                                  ///< 索引名
        bool user_set_name = false;                           ///< 用户是否设置了索引名(未设置的情况下使用field生成)
        bool unique_ = false;                                 ///< 是否唯一索引
        std::vector<std::tuple<std::string, Order>> fields_{};///< 字段名及排序方式

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

        Index() = default;
        Index(std::string field, Order order = ASC);
        Index(std::string table, std::string field, Order order = ASC);
        Index &table(std::optional<std::string> table);
        Index &name(std::string name);
        Index &unique(bool unique = true);
        Index &field(std::string field, Order order = ASC);
        Index &fields(const std::vector<std::tuple<std::string, Order>> &append);
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_INDEX__HPP_GUARD
