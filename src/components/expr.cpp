#include "sqlcpp/components/expr.hpp"
#include "sqlcpp/components/cond.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/select.hpp"
#include "sqlcpp/components/value.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>
namespace sqlcpp {
    Expr::Expr(InnerValue v) : value_(std::move(v)) {}
    Expr::Expr(OpExpr expr) : value_(std::make_shared<OpExpr>(std::move(expr))) {}
    Expr::Expr(CaseExpr expr) : value_(std::make_shared<CaseExpr>(std::move(expr))) {}
    Expr::Expr(FuncExpr expr) : value_(std::make_shared<FuncExpr>(std::move(expr))) {}
    Expr::Expr(FieldLike field) : value_(std::make_shared<FieldLike>(std::move(field))) {}
    Expr::Expr(Field field) : value_(std::make_shared<FieldLike>(std::move(field))) {}
    Expr::Expr(std::string table, std::string field) : value_(std::make_shared<FieldLike>(std::move(table), std::move(field))) {}
    Expr::Expr(std::string table, std::string field, std::string alias) : value_(std::make_shared<FieldLike>(std::move(table), std::move(field), std::move(alias))) {}
    Expr::Expr(RawField field) : value_(std::make_shared<FieldLike>(std::move(field))) {}
    Expr::Expr(ValueLike value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(Value value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(RawValue value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(NullValue value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(IndexedVarValue value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(VarValue value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(BlobValue value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(const char *value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(std::string value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(int64_t value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(uint64_t value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(double value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(float value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(bool value) : value_(std::make_shared<ValueLike>(std::move(value))) {}
    Expr::Expr(std::nullptr_t) : value_(std::make_shared<ValueLike>(nullptr)) {}
    Expr::Expr(std::nullopt_t) : value_(std::make_shared<ValueLike>(std::nullopt)) {}

    Expr &Expr::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    Expr &Expr::as(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }

    void Expr::build_s(std::ostream &oss, const Type &t) const {
        if (alias_) oss << '(';
        if (auto *select = std::get_if<std::shared_ptr<Select>>(&value_); select) {
            (*select)->build_s(oss, t, true);// select 语句特殊化: 指定为子查询, 即抛弃结尾的分号
        } else {
            std::visit([&](auto &&arg) { arg->build_s(oss, t); }, value_);
        }
        if (alias_) oss << ") AS " << *alias_;
    }
    void Expr::edit_var_map(VarMap &var_map) const {
        std::visit([&](auto &&arg) { arg->edit_var_map(var_map); }, value_);
    }
    OpExpr::OpExpr(Op op, Expr a, Expr b) : a(std::move(a)), b(std::move(b)), op(op) {}

    OpExpr &OpExpr::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    OpExpr &OpExpr::as(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }

    void OpExpr::build_s(std::ostream &oss, const Type &t) const {
        if (alias_) oss << '(';
        a.build_s(oss, t);
        oss << op_name(op);
        b.build_s(oss, t);
        if (alias_) oss << ") AS " << *alias_;
    }
    void OpExpr::edit_var_map(VarMap &var_map) const {
        a.edit_var_map(var_map);
        b.edit_var_map(var_map);
    }


    CaseExpr &CaseExpr::set_expr(Expr expr) {
        expr_ = std::move(expr);
        return *this;
    }
    CaseExpr &CaseExpr::add_case(Condition cond, Expr expr) {
        cases_.emplace_back(std::make_shared<Condition>(std::move(cond)), std::move(expr));
        return *this;
    }
    CaseExpr &CaseExpr::add_case(Expr cond, Expr expr) {
        cases_.emplace_back(std::move(cond), std::move(expr));
        return *this;
    }
    CaseExpr &CaseExpr::set_else(Expr expr) {
        else_ = std::move(expr);
        return *this;
    }
    CaseExpr &CaseExpr::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    CaseExpr &CaseExpr::as(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    void CaseExpr::build_s(std::ostream &oss, const Type &t) const {
        if (alias_) oss << '(';
        oss << "CASE ";
        if (expr_) {
            expr_->build_s(oss, t);
        }
        bool isExprMode = bool(expr_);
        for (auto &c: cases_) {
            if (isExprMode) {
                if (!std::holds_alternative<Expr>(c.first))
                    throw std::invalid_argument("[sqlcpp::CaseExpr] expr mode only support Expr as condition");
                std::get<Expr>(c.first).build_s(oss, t);
            } else {
                if (!std::holds_alternative<std::shared_ptr<Condition>>(c.first))
                    throw std::invalid_argument("[sqlcpp::CaseExpr] cond mode only support Condition as condition");
                std::get<std::shared_ptr<Condition>>(c.first)->build_s(oss, t);
            }
            oss << " THEN ";
            c.second.build_s(oss, t);
        }
        if (else_) {
            oss << " ELSE ";
            else_->build_s(oss, t);
        }
        oss << " END";
        if (alias_) oss << ") AS " << *alias_;
    }
    void CaseExpr::edit_var_map(VarMap &var_map) const {
        for (auto &c: cases_) {
            if (auto *cond = std::get_if<std::shared_ptr<Condition>>(&c.first); cond) {
                (*cond)->edit_var_map(var_map);
            } else if (auto *expr = std::get_if<Expr>(&c.first); expr) {
                expr->edit_var_map(var_map);
            }
            c.second.edit_var_map(var_map);
        }
        if (else_) {
            else_->edit_var_map(var_map);
        }
    }
    FuncExpr &FuncExpr::set_name(std::string name) {
        name_ = std::move(name);
        return *this;
    }
    FuncExpr &FuncExpr::add_arg(Expr arg) {
        args_.emplace_back(std::move(arg));
        return *this;
    }
    FuncExpr &FuncExpr::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    FuncExpr &FuncExpr::as(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    void FuncExpr::build_s(std::ostream &oss, const Type &t) const {
        oss << name_ << '(';
        for (size_t i = 0; i < args_.size(); ++i) {
            if (i > 0) oss << ',';
            args_[i].build_s(oss, t);
        }
        oss << ')';
        if (alias_) oss << " AS " << *alias_;
    }
    void FuncExpr::edit_var_map(VarMap &var_map) const {
        for (auto &arg: args_) {
            arg.edit_var_map(var_map);
        }
    }
}// namespace sqlcpp
