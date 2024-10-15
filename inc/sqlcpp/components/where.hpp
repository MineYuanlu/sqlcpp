/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_WHERE__HPP_GUARD
#define SQLCPP_COMPONENTS_WHERE__HPP_GUARD
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/defs.hpp"
namespace sqlcpp {

    struct Where : Builder {
        Condition condition_;

        Where(Condition condition);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_WHERE__HPP_GUARD
