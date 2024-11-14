#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/assign.hpp"
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
    Field &Field::distinct(bool v) {
        distinct_ = v;
        return *this;
    }

    OrderByField Field::asc() const {
        return {*this, Order::ASC};
    }
    OrderByField Field::desc() const {
        return {*this, Order::DESC};
    }

    void Field::build_s(std::ostream &oss, const Type &t) const {
        if (distinct_) oss << "DISTINCT ";
        if (table_name_) oss << safe_table(*table_name_, t) << ".";
        oss << safe_field(field_name_, t);
        if (alias_) oss << " AS " << safe_field(*alias_, t);
    }
    void Field::edit_var_map([[maybe_unused]] VarMap &var_map) const {
        // nothing to do
    }

    CondCmp Field::LIKE(ValueLike v) const {
        return {*this, CmpOp::LIKE, v};
    }
    CondCmp Field::NOT_LIKE(ValueLike v) const {
        return {*this, CmpOp::NOT_LIKE, v};
    }
    CondIn Field::IN(std::vector<ValueLike> vs) const {
        return {*this, vs};
    }
    CondNotIn Field::NOT_IN(std::vector<ValueLike> vs) const {
        return {*this, vs};
    }
    CondBetween Field::BETWEEN(ValueLike start, ValueLike end) const {
        return {*this, start, end};
    }
    Assign Field::assign(ValueLike v) const {
        return {*this, std::move(v)};
    }
    Assign Field::operator=(ValueLike v) const {
        return assign(std::move(v));
    }


    RawField::RawField(std::string raw_field) : raw_field_(std::move(raw_field)) {}
    void RawField::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << raw_field_;
    }
    void RawField::edit_var_map([[maybe_unused]] VarMap &var_map) const {
        // nothing to do
    }


    FieldLike::FieldLike(const char *field) : FieldLike(Field{std::move(field)}) {}
    FieldLike::FieldLike(std::string field) : FieldLike(Field{std::move(field)}) {}
    FieldLike::FieldLike(std::string table, std::string field) : FieldLike(Field{std::move(table), std::move(field)}) {}
    FieldLike::FieldLike(std::string table, std::string field, std::string alias) : FieldLike(Field{std::move(table), std::move(field), std::move(alias)}) {}
    FieldLike::FieldLike(Field field) : field_(std::move(field)) {}
    FieldLike::FieldLike(RawField field) : field_(std::move(field)) {}
    FieldLike::FieldLike(Expr field) : field_(std::move(field)) {}
    void FieldLike::build_s(std::ostream &oss, const Type &t) const {
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, field_);
    }
    void FieldLike::edit_var_map(VarMap &var_map) const {
        std::visit([&](const auto &arg) { arg.edit_var_map(var_map); }, field_);
    }
    CondCmp FieldLike::LIKE(ValueLike v) const {
        return {*this, CmpOp::LIKE, v};
    }
    CondCmp FieldLike::NOT_LIKE(ValueLike v) const {
        return {*this, CmpOp::NOT_LIKE, v};
    }
    CondIn FieldLike::IN(std::vector<ValueLike> vs) const {
        return {*this, vs};
    }
    CondNotIn FieldLike::NOT_IN(std::vector<ValueLike> vs) const {
        return {*this, vs};
    }
    CondBetween FieldLike::BETWEEN(ValueLike start, ValueLike end) const {
        return {*this, start, end};
    }
    Assign FieldLike::assign(ValueLike v) const {
        return {*this, std::move(v)};
    }
    Assign FieldLike::operator=(ValueLike v) const {
        return assign(std::move(v));
    }
}// namespace sqlcpp
