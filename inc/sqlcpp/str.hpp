/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_STR__HPP_GUARD
#define SQLCPP_STR__HPP_GUARD
#include "sqlcpp/types.hpp"
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
namespace sqlcpp::str {
    [[nodiscard]] std::string package_string(const std::string &input, char package);
    [[nodiscard]] std::string join(const std::initializer_list<std::string> &list, const std::string &joiner,
                                   const std::function<std::string(std::string)> &val_trans,
                                   const std::string &prefix = "", const std::string &suffix = "");

    [[nodiscard]] std::string safe_table(const std::string &table, const Type &t);
    [[nodiscard]] std::string safe_field(const std::string &field, const Type &t);
    [[nodiscard]] std::string safe_value(const std::string &value);
    [[nodiscard]] std::string safe_values(const std::initializer_list<std::string> &value, const std::string &joiner = ",");

    [[nodiscard]] std::string toUpperCase(std::string str);
    [[nodiscard]] std::string toLowerCase(std::string str);

    void hex_stream(std::ostream &os, const unsigned char *const data, const size_t data_len, const size_t buffer = 1024);
}// namespace sqlcpp::str
#endif// SQLCPP_STR__HPP_GUARD
