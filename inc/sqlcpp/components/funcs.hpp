/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
#define SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include <string>
namespace sqlcpp::F {
    struct NamedFunc {
        std::string name_;
        FuncField operator()(Field arg) const;
        FuncField operator()(RawField arg) const;
    };
    static const NamedFunc COUNT = {"COUNT"};
    static const NamedFunc SUM = {"SUM"};
    static const NamedFunc AVG = {"AVG"};
    static const NamedFunc MIN = {"MIN"};
    static const NamedFunc MAX = {"MAX"};
    static const NamedFunc LENGTH = {"LENGTH"};
    static const NamedFunc SUBSTR = {"SUBSTR"};
    static const NamedFunc UPPER = {"UPPER"};
    static const NamedFunc LOWER = {"LOWER"};
    static const NamedFunc TRIM = {"TRIM"};
    static const NamedFunc LTRIM = {"LTRIM"};
    static const NamedFunc RTRIM = {"RTRIM"};
    static const NamedFunc REPLACE = {"REPLACE"};
    static const NamedFunc DATE = {"DATE"};
    static const NamedFunc DATETIME = {"DATETIME"};
    static const NamedFunc TIME = {"TIME"};
    static const NamedFunc YEAR = {"YEAR"};
    static const NamedFunc MONTH = {"MONTH"};
    static const NamedFunc DAY = {"DAY"};
    static const NamedFunc HOUR = {"HOUR"};
    static const NamedFunc MINUTE = {"MINUTE"};
    static const NamedFunc SECOND = {"SECOND"};
    static const NamedFunc MICROSECOND = {"MICROSECOND"};
    static const NamedFunc NOW = {"NOW"};
    static const NamedFunc CURRENT_DATE = {"CURRENT_DATE"};
    static const NamedFunc CURRENT_TIME = {"CURRENT_TIME"};
    static const NamedFunc CURRENT_TIMESTAMP = {"CURRENT_TIMESTAMP"};
    static const NamedFunc EXTRACT = {"EXTRACT"};
    static const NamedFunc DATE_PART = {"DATE_PART"};
    static const NamedFunc DATE_TRUNC = {"DATE_TRUNC"};
    static const NamedFunc TIMESTAMP_TRUNC = {"TIMESTAMP_TRUNC"};
    static const NamedFunc STRFTIME = {"STRFTIME"};
    static const NamedFunc GROUP_CONCAT = {"GROUP_CONCAT"};
    static const NamedFunc EXISTS = {"EXISTS"};
    static const NamedFunc NOT_EXISTS = {"NOT EXISTS"};
    static const NamedFunc COALESCE = {"COALESCE"};
    static const NamedFunc VALUES = {"VALUES"};
}// namespace sqlcpp::F
#endif// SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
