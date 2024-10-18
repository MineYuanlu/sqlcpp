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
namespace sqlcpp {


    struct Builder {

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const = 0;
        std::string build(const Type &t = SQLITE) const {
            std::ostringstream oss;
            build_s(oss, t);
            return oss.str();
        }
    };
    struct NoAliasBuilder {
        virtual void build_no_alias(std::ostream &oss, const Type &t = SQLITE) const = 0;
    };
}// namespace sqlcpp
#endif// SQLCPP_DEFS__HPP_GUARD
