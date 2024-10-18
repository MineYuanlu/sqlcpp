/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
#define SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/value.hpp"
#include "sqlcpp/defs.hpp"
namespace sqlcpp {
    struct Assigns;
    struct Assign final : Builder {
        FieldLike field_;
        ValueLike value_;

        Assign(FieldLike field, ValueLike value = VAR);

        Assigns add(Assign assign) const;
        Assigns operator+(Assign assign) const;


        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    struct Assigns final : Builder {
        std::vector<Assign> assigns_;

        Assigns() = default;
        Assigns(FieldLike field, ValueLike value = VAR);
        Assigns(Assign assign);
        Assigns(std::vector<Assign> assigns);


        Assigns &add(Assign assign);

        Assigns &operator+=(Assign assign);
        Assigns &operator+=(const Assigns &assign);
        Assigns operator+(Assign assign) const &;
        Assigns &operator+(Assign assign) &&;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
