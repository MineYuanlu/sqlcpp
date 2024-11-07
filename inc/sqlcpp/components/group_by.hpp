/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_GROUP_BY__HPP_GUARD
#define SQLCPP_COMPONENTS_GROUP_BY__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/defs.hpp"
#include <vector>
namespace sqlcpp {


    /// @brief 分组组件, 按某一列或多列的值进行分组, 将具有相同值的行归为一组
    struct GroupBy final : public Builder, public VarBuilder {
        std::vector<FieldLike> fields_;///< 分组的列

        /// @brief 使用单个列构造
        GroupBy(FieldLike field);
        /// @brief 使用多个列构造
        GroupBy(std::vector<FieldLike> fields);
        /// @brief 使用多个列构造
        template<typename... Args>
        GroupBy(FieldLike field, Args &&...args) : fields_({std::move(field)}) {
            by(std::forward<Args>(args)...);
        }
        /// @brief 添加分组列
        GroupBy &by(FieldLike field);
        /// @brief 添加分组列
        template<typename... Args>
        inline GroupBy &by(FieldLike field, Args &&...args) {
            by(std::move(field));
            return by(std::forward<Args>(args)...);
        }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_GROUP_BY__HPP_GUARD
