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

    enum class Types {
        MYSQL,
        SQLITE,
    };
    struct Type {
        Types type;
        std::string_view name;
        char quote_table;
        char quote_field;
        char quote_value;
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
