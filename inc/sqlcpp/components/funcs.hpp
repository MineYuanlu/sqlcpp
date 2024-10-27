/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
#define SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include <string>
/// @namespace sqlcpp::F
/// @brief sqlcpp的方言函数组件
namespace sqlcpp::F {
    /// @brief 预命名的函数
    struct NamedFunc final {
        /// 函数名
        std::string name_;
        /// @brief 用此函数包裹一个字段
        FuncField operator()(Field arg) const;
        /// @brief 用此函数包裹一个raw字段
        FuncField operator()(RawField arg) const;
    };
    static const NamedFunc COUNT = {"COUNT"};                        ///< COUNT: 统计符合条件的记录数。
    static const NamedFunc SUM = {"SUM"};                            ///< SUM: 计算数值列的总和。
    static const NamedFunc AVG = {"AVG"};                            ///< AVG: 计算数值列的平均值。
    static const NamedFunc MIN = {"MIN"};                            ///< MIN: 返回指定列的最小值。
    static const NamedFunc MAX = {"MAX"};                            ///< MAX: 返回指定列的最大值。
    static const NamedFunc LENGTH = {"LENGTH"};                      ///< LENGTH: 返回字符串的字符长度。
    static const NamedFunc SUBSTRING = {"SUBSTRING"};                ///< SUBSTRING: 提取字符串的子字符串，在MySQL中为SUBSTRING，SQLite中为SUBSTR。
    static const NamedFunc SUBSTR = {"SUBSTR"};                      ///< SUBSTR: 提取字符串的子字符串，在MySQL中为SUBSTRING，SQLite中为SUBSTR。
    static const NamedFunc UPPER = {"UPPER"};                        ///< UPPER: 将字符串转换为大写。
    static const NamedFunc LOWER = {"LOWER"};                        ///< LOWER: 将字符串转换为小写。
    static const NamedFunc TRIM = {"TRIM"};                          ///< TRIM: 去除字符串两端的空格或指定字符。
    static const NamedFunc LTRIM = {"LTRIM"};                        ///< LTRIM: 去除字符串左侧的空格或指定字符。
    static const NamedFunc RTRIM = {"RTRIM"};                        ///< RTRIM: 去除字符串右侧的空格或指定字符。
    static const NamedFunc REPLACE = {"REPLACE"};                    ///< REPLACE: 将字符串中的指定子字符串替换为新字符串。
    static const NamedFunc DATE = {"DATE"};                          ///< DATE: 提取日期部分（MySQL和SQLite中都可使用，但实现略有不同）。
    static const NamedFunc DATETIME = {"DATETIME"};                  ///< DATETIME: 返回日期和时间。
    static const NamedFunc TIME = {"TIME"};                          ///< TIME: 提取时间部分。
    static const NamedFunc YEAR = {"YEAR"};                          ///< YEAR: 提取日期的年份部分。
    static const NamedFunc MONTH = {"MONTH"};                        ///< MONTH: 提取日期的月份部分。
    static const NamedFunc DAYOFMONTH = {"DAYOFMONTH"};              ///< DAYOFMONTH: 提取日期的日期部分，在MySQL中为DAYOFMONTH，SQLite中为DAY。
    static const NamedFunc DAY = {"DAY"};                            ///< DAY: 提取日期的日期部分，在MySQL中为DAYOFMONTH，SQLite中为DAY。
    static const NamedFunc HOUR = {"HOUR"};                          ///< HOUR: 提取时间的小时部分。
    static const NamedFunc MINUTE = {"MINUTE"};                      ///< MINUTE: 提取时间的分钟部分。
    static const NamedFunc SECOND = {"SECOND"};                      ///< SECOND: 提取时间的秒部分。
    static const NamedFunc MICROSECOND = {"MICROSECOND"};            ///< MICROSECOND: 提取时间的微秒部分（MySQL支持，SQLite需要扩展库支持）。
    static const NamedFunc NOW = {"NOW"};                            ///< NOW: 返回当前的日期和时间。
    static const NamedFunc CURRENT_DATE = {"CURRENT_DATE"};          ///< CURRENT_DATE: 返回当前的日期。
    static const NamedFunc CURRENT_TIME = {"CURRENT_TIME"};          ///< CURRENT_TIME: 返回当前的时间。
    static const NamedFunc CURRENT_TIMESTAMP = {"CURRENT_TIMESTAMP"};///< CURRENT_TIMESTAMP: 返回当前的日期和时间（等同于NOW）。
    static const NamedFunc EXTRACT = {"EXTRACT"};                    ///< EXTRACT: 从日期或时间中提取特定部分（如年、月、日等）。
    static const NamedFunc DATE_PART = {"DATE_PART"};                ///< DATE_PART: 提取日期的指定部分，类似EXTRACT（仅MySQL）。
    static const NamedFunc DATE_TRUNC = {"DATE_TRUNC"};              ///< DATE_TRUNC: 截断日期到指定精度（SQLite需要扩展库支持）。
    static const NamedFunc TIMESTAMP_TRUNC = {"TIMESTAMP_TRUNC"};    ///< TIMESTAMP_TRUNC: 截断时间戳到指定精度（MySQL中需自定义函数实现）。
    static const NamedFunc STRFTIME = {"STRFTIME"};                  ///< STRFTIME: 将日期格式化为指定格式（SQLite支持MySQL不支持，MySQL中需使用DATE_FORMAT）。
    static const NamedFunc GROUP_CONCAT = {"GROUP_CONCAT"};          ///< GROUP_CONCAT: 将一组值连接成字符串。
    static const NamedFunc EXISTS = {"EXISTS"};                      ///< EXISTS: 判断子查询是否返回结果，存在则返回true。
    static const NamedFunc NOT_EXISTS = {"NOT EXISTS"};              ///< NOT EXISTS: 判断子查询是否没有结果，不存在则返回true。
    static const NamedFunc COALESCE = {"COALESCE"};                  ///< COALESCE: 返回第一个非NULL的值。
    static const NamedFunc VALUES = {"VALUES"};                      ///< VALUES: 在INSERT中插入的值。


}// namespace sqlcpp::F
#endif// SQLCPP_COMPONENTS_FUNCS__HPP_GUARD
