/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_TYPES__HPP_GUARD
#define SQLCPP_TYPES__HPP_GUARD

#include <string>
#include <string_view>
#if __has_include(<drogon/orm/SqlBinder.h>)
#include "drogon/orm/SqlBinder.h"
#include <stdexcept>
#endif
namespace sqlcpp {

    /// @brief sqlcpp所支持的数据库类型
    enum class Types {
        MYSQL, ///< MySQL数据库
        SQLITE,///< SQLite数据库
    };
    /// @brief sqlcpp所支持的数据库类型信息
    /// @details 包含数据库类型、名称、表名、字段名、值名的引用符号
    struct Type {
        Types type;           ///< 数据库类型
        std::string_view name;///< 数据库名称
        char quote_table;     ///< 表名引用符号
        char quote_field;     ///< 字段名引用符号
        char quote_value;     ///< 值名引用符号
        constexpr Type(Types type, std::string_view name, char quote_table, char quote_field, char quote_value)
            : type(type), name(name), quote_table(quote_table), quote_field(quote_field), quote_value(quote_value) {}

#if __cpp_lib_three_way_comparison
        inline auto operator<=>(const Type &t) const { return name <=> t.name; };
#else
        inline bool operator<(const Type &t) const { return name < t.name; }
        inline bool operator>(const Type &t) const { return name > t.name; }
        inline bool operator<=(const Type &t) const { return !(name > t.name); }
        inline bool operator>=(const Type &t) const { return !(name < t.name); }
        inline bool operator==(const Type &t) const { return name == t.name; }
        inline bool operator!=(const Type &t) const { return name != t.name; }
#endif

        inline operator std::string() const { return std::string{name}; }
        inline operator Types() const { return type; }
#if __has_include(<drogon/orm/SqlBinder.h>)
        Type(const drogon::orm::ClientType &t);
#endif
    };
    static constexpr const Type MYSQL{Types::MYSQL, "MYSQL", '`', '`', '\''};
    static constexpr const Type SQLITE{Types::SQLITE, "SQLITE", '"', '"', '\''};
#if __has_include(<drogon/orm/SqlBinder.h>)
    /// @brief 将drogon::orm::ClientType转换为sqlcpp::Type
    static const Type &trans(const drogon::orm::ClientType &t) {
        switch (t) {
            case drogon::orm::ClientType::Mysql:
                return MYSQL;
            case drogon::orm::ClientType::Sqlite3:
                return SQLITE;
            case drogon::orm::ClientType::PostgreSQL:
                break;
        }
        throw std::runtime_error("[sqlcpp] Unsupport Db Client Type " + std::to_string(static_cast<int>(t)));
    }

    inline Type::Type(const drogon::orm::ClientType &t) : Type(trans(t)) {}
#endif
}// namespace sqlcpp
#endif// SQLCPP_TYPES__HPP_GUARD
