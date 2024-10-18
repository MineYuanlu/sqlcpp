/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FROM__HPP_GUARD
#define SQLCPP_COMPONENTS_FROM__HPP_GUARD
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/defs.hpp"
#include <string>
#include <variant>
namespace sqlcpp {
    enum JoinType {
        INNER_JOIN,
        LEFT_JOIN,
        RIGHT_JOIN,
        FULL_JOIN,
    };
    struct Froms;
    struct RawFrom;

    struct From final : public Builder {
        std::string table_name_;
        std::optional<std::string> alias_{};

        From(const char *table);
        From(std::string table);
        From(std::string table, std::optional<std::string> alias);
        From &alias(std::string alias);

        Field operator[](std::string field) const;
        Froms join(JoinType t, From from, Condition cond) const;
        Froms join(JoinType t, RawFrom from, Condition cond) const;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    struct RawFrom final : public Builder {
        std::string raw_from_;

        explicit RawFrom(std::string from);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    constexpr inline const char *join_type_str(JoinType type) {
        // clang-format off
        switch (type) {
            case INNER_JOIN: return "INNER JOIN";
            case LEFT_JOIN: return "LEFT JOIN";
            case RIGHT_JOIN: return "RIGHT JOIN";
            case FULL_JOIN: return "FULL OUTER JOIN";
        }
        // clang-format on
        return "";
    }


    struct Froms final : public Builder {
        std::variant<From, RawFrom> from_;
        std::vector<std::tuple<JoinType, std::variant<From, RawFrom>, Condition>> joins_;

        Froms(From);
        Froms(RawFrom);
        Froms &join(JoinType t, From from, Condition cond);
        Froms &join(JoinType t, RawFrom from, Condition cond);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_FROM__HPP_GUARD
