/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_WHERE__HPP_GUARD
#define SQLCPP_COMPONENTS_WHERE__HPP_GUARD
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/defs.hpp"
namespace sqlcpp {

    /// @brief 筛选组件
    struct Where final : public Builder, public VarBuilder {
        Condition condition_;///< 条件表达式

        Where(Condition condition);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_WHERE__HPP_GUARD
