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
/// @namespace sqlcpp::str
/// @brief sqlcpp的字符串工具
namespace sqlcpp::str {
    /// @brief 将input使用package包装, 且将input内含有的package字符替换为\package
    /// @param input 输入字符串
    /// @param package 包装字符
    /// @return 包装后的字符串
    [[nodiscard]] std::string package_string(const std::string &input, char package);
    /// @brief 将list中元素使用val_trans转换后, 使用joiner连接, 并添加prefix和suffix
    [[nodiscard]] std::string join(const std::initializer_list<std::string> &list, const std::string &joiner,
                                   const std::function<std::string(std::string)> &val_trans,
                                   const std::string &prefix = "", const std::string &suffix = "");

    /// @brief 将table针对t进行安全处理
    /// @param table 表名
    /// @param t 数据库类型
    [[nodiscard]] std::string safe_table(const std::string &table, const Type &t);
    /// @brief 将field针对t进行安全处理
    /// @param field 字段名
    /// @param t 数据库类型
    [[nodiscard]] std::string safe_field(const std::string &field, const Type &t);
    /// @brief 将value针对t进行安全处理
    /// @param value 值
    [[nodiscard]] std::string safe_value(const std::string &value);
    /// @brief 将values针对t进行安全处理
    /// @param value 值列表
    /// @param joiner 连接符
    /// @return 安全处理后的字符串
    [[nodiscard]] std::string safe_values(const std::initializer_list<std::string> &value, const std::string &joiner = ",");

    /// @brief 将字符串转换为大写
    [[nodiscard]] std::string toUpperCase(std::string str);
    /// @brief 将字符串转换为小写
    [[nodiscard]] std::string toLowerCase(std::string str);

    /// @brief 将data转换为十六进制字符串
    /// @param os 十六进制输出流
    /// @param data 二进制数据
    /// @param data_len 数据长度
    /// @param buffer 缓冲区大小, 一次性最多对os写入的字节数
    void hex_stream(std::ostream &os, const unsigned char *const data, const size_t data_len, const size_t buffer = 1024);
}// namespace sqlcpp::str
#endif// SQLCPP_STR__HPP_GUARD
