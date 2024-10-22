/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_CHECK__HPP_GUARD
#define SQLCPP_COMPONENTS_CHECK__HPP_GUARD
#include "sqlcpp/defs.hpp"
namespace sqlcpp {
    /// @brief Table check component.
    struct Check final : public Builder {
        std::string check_{};
        Check(std::string check);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_CHECK__HPP_GUARD
