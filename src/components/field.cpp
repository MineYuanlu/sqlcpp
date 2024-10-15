#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/components/order_by.hpp"
#include "sqlcpp/components/value.hpp"
#include "sqlcpp/str.hpp"
namespace sqlcpp {
    using namespace sqlcpp::str;

    Field::Field(const char *field_name)
        : field_name_(field_name) {}
    Field::Field(std::string field_name)
        : field_name_(std::move(field_name)) {}
    Field::Field(std::string table_name, std::string field_name)
        : table_name_(std::move(table_name)), field_name_(std::move(field_name)) {}
    Field::Field(std::string table_name, std::string field_name, std::string alias)
        : table_name_(std::move(table_name)), field_name_(std::move(field_name)), alias_(std::move(alias)) {}

    Field &Field::table(std::string table_name) {
        table_name_ = std::move(table_name);
        return *this;
    }
    Field &Field::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    Field &Field::as(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }

    OrderByField Field::asc() const {
        return {*this, Order::ASC};
    }
    OrderByField Field::desc() const {
        return {*this, Order::DESC};
    }

    void Field::build_s(std::ostream &oss, const Type &t) const {
        if (table_name_) oss << safe_table(*table_name_, t) << ".";
        oss << safe_field(field_name_, t);
        if (alias_) oss << " AS " << safe_field(*alias_, t);
    }


    RawField::RawField(std::string raw_field) : raw_field_(std::move(raw_field)) {}
    void RawField::build_s(std::ostream &oss, const Type &t) const {
        oss << raw_field_;
    }


    FuncField::FuncField(std::string func_name, std::variant<Field, RawField> args)
        : func_name_(std::move(func_name)), args_(std::move(args)) {}
    FuncField::FuncField(std::string func_name, std::variant<Field, RawField> args, std::string alias)
        : func_name_(std::move(func_name)), args_(std::move(args)), alias_(std::move(alias)) {}
    FuncField &FuncField::alias(std::string alias) {
        this->alias_ = std::move(alias);
        return *this;
    }
    void FuncField::build_s(std::ostream &oss, const Type &t) const {
        oss << func_name_ << "(";
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, args_);
        oss << ")";
        if (alias_) oss << " AS " << safe_field(*alias_, t);
    }


    FieldLike::FieldLike(Field field) : field_(std::move(field)) {}
    FieldLike::FieldLike(RawField field) : field_(std::move(field)) {}
    FieldLike::FieldLike(FuncField field) : field_(std::move(field)) {}
    void FieldLike::build_s(std::ostream &oss, const Type &t) const {
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, field_);
    }
    CondCmp FieldLike::LIKE(ValueLike v) {
        return {*this, CmpOp::LIKE, v};
    }
    CondCmp FieldLike::NOT_LIKE(ValueLike v) {
        return {*this, CmpOp::NOT_LIKE, v};
    }
    CondIn FieldLike::IN(std::vector<ValueLike> vs) {
        return {*this, vs};
    }
    CondNotIn FieldLike::NOT_IN(std::vector<ValueLike> vs) {
        return {*this, vs};
    }
    CondBetween FieldLike::BETWEEN(ValueLike start, ValueLike end) {
        return {*this, start, end};
    }
}// namespace sqlcpp
