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


    struct GroupBy final : public Builder {
        std::vector<FieldLike> fields_;

        GroupBy(FieldLike field);
        GroupBy(std::vector<FieldLike> fields);
        template<typename... Args>
        GroupBy(FieldLike field, Args &&...args) : fields_({std::move(field)}) {
            by(std::forward<Args>(args)...);
        }
        GroupBy &by(FieldLike field);
        template<typename... Args>
        inline GroupBy &by(FieldLike field, Args &&...args) {
            by(std::move(field));
            return by(std::forward<Args>(args)...);
        }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_GROUP_BY__HPP_GUARD
