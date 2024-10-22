/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_CHECK__HPP_GUARD
#define SQLCPP_COMPONENTS_CHECK__HPP_GUARD
#include "sqlcpp/defs.hpp"
namespace sqlcpp {
    /// @brief CHECK约束，用于定义数据表中的检查条件
    struct Check final : public Builder {
        /// @brief CHECK约束的表达式
        std::string check_{};
        /// @brief 构造CHECK
        Check(std::string check);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_CHECK__HPP_GUARD
