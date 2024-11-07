/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_HAVING__HPP_GUARD
#define SQLCPP_COMPONENTS_HAVING__HPP_GUARD
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/defs.hpp"
namespace sqlcpp {


    /// @brief 过滤组件, 基于聚合后的数据来进行条件筛选
    struct Having final : public Builder, public VarBuilder {
        Condition condition_;///< 条件表达式

        Having(Condition condition);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_HAVING__HPP_GUARD
