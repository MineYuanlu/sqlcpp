/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_FIELD__HPP_GUARD
#define SQLCPP_COMPONENTS_FIELD__HPP_GUARD


#include "sqlcpp/defs.hpp"
#include <optional>
#include <variant>
#include <vector>
namespace sqlcpp {

    struct OrderByField;
    struct ValueLike;
    struct CondCmp;
    struct CondIn;
    struct CondNotIn;
    struct CondBetween;
    struct Assign;

    struct Field : Builder {
        std::optional<std::string> table_name_{};
        std::string field_name_;
        std::optional<std::string> alias_{};

        Field(const char *field);
        Field(std::string field);
        Field(std::string table, std::string field);
        Field(std::string table, std::string field, std::string alias);
        Field &table(std::string table);
        Field &alias(std::string alias);
        Field &as(std::string alias);

        OrderByField asc() const;
        OrderByField desc() const;

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;

        Assign assign(ValueLike) const;
        Assign operator=(ValueLike) const;
    };


    struct RawField : Builder {
        std::string raw_field_;
        explicit RawField(std::string field);
        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    struct FuncField : Builder {
        std::string func_name_;
        std::variant<Field, RawField> args_;
        std::optional<std::string> alias_{};

        FuncField(std::string func, std::variant<Field, RawField> args);
        FuncField(std::string func, std::variant<Field, RawField> args, std::string alias);
        FuncField &alias(std::string alias);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };

    struct FieldLike : Builder {
        std::variant<Field, RawField, FuncField> field_;
        FieldLike(const char *field);
        FieldLike(std::string field);
        FieldLike(std::string table, std::string field);
        FieldLike(std::string table, std::string field, std::string alias);
        FieldLike(Field field);
        FieldLike(RawField field);
        FieldLike(FuncField field);
        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
        CondCmp LIKE(ValueLike v) const;
        CondCmp NOT_LIKE(ValueLike v) const;
        CondIn IN(std::vector<ValueLike> vs) const;
        CondNotIn NOT_IN(std::vector<ValueLike> vs) const;
        CondBetween BETWEEN(ValueLike start, ValueLike end) const;

        Assign assign(ValueLike) const;
        Assign operator=(ValueLike) const;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_FIELD__HPP_GUARD
