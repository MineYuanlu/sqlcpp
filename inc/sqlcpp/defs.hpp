/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_DEFS__HPP_GUARD
#define SQLCPP_DEFS__HPP_GUARD
#include "sqlcpp/types.hpp"
#include <ostream>
#include <sstream>
#include <string>
/// @namespace sqlcpp
/// @brief sqlcpp所有代码的基础命名空间
namespace sqlcpp {

    static constexpr const auto SQLCPP_DEFAULT_TYPE = SQLITE;
    struct Builder {

        virtual void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const = 0;
        std::string build(const Type &t = SQLCPP_DEFAULT_TYPE) const {
            std::ostringstream oss;
            build_s(oss, t);
            return oss.str();
        }
    };
}// namespace sqlcpp
#endif// SQLCPP_DEFS__HPP_GUARD
