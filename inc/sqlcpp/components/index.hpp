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
    struct Index final : public Builder {
        std::optional<std::string> table_{};
        std::string name_{};
        bool user_set_name = false;
        bool unique_ = false;
        std::vector<std::tuple<std::string, Order>> fields_{};
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
