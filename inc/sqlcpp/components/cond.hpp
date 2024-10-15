/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_COND__HPP_GUARD
#define SQLCPP_COMPONENTS_COND__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/value.hpp"
#include "sqlcpp/defs.hpp"
#include <memory>
namespace sqlcpp {
    /// @brief 比较运算符
    enum CmpOp {
        EQ,
        NEQ,
        GT,
        GTE,
        LT,
        LTE,
        LIKE,
        NOT_LIKE,
    };
    constexpr inline const char *cmp_op_str(CmpOp op) {
        // clang-format off
        switch (op) {
            case EQ: return "=";
            case NEQ: return "!=";
            case GT: return ">";
            case GTE: return ">=";
            case LT: return "<";
            case LTE: return "<=";
            case LIKE: return "LIKE";
            case NOT_LIKE: return "NOT LIKE";
        }
        // clang-format on
        return "";
    }


    struct CondOp;
    struct Condition;
    /// @brief 条件构建器接口
    struct Cond {
        virtual CondOp AND(const Condition &c);
        virtual CondOp OR(const Condition &c);
        virtual CondOp NOT();
    };


    /// @brief 比较条件
    struct CondCmp : Builder, Cond {
        FieldLike field_;
        CmpOp op_;
        ValueLike value_;

        CondCmp(FieldLike field, CmpOp op, ValueLike value);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    inline CondCmp operator==(FieldLike &&field, ValueLike &&value) { return {std::move(field), EQ, std::move(value)}; }
    inline CondCmp operator!=(FieldLike &&field, ValueLike &&value) { return {std::move(field), NEQ, std::move(value)}; }
    inline CondCmp operator>(FieldLike &&field, ValueLike &&value) { return {std::move(field), GT, std::move(value)}; }
    inline CondCmp operator>=(FieldLike &&field, ValueLike &&value) { return {std::move(field), GTE, std::move(value)}; }
    inline CondCmp operator<(FieldLike &&field, ValueLike &&value) { return {std::move(field), LT, std::move(value)}; }
    inline CondCmp operator<=(FieldLike &&field, ValueLike &&value) { return {std::move(field), LTE, std::move(value)}; }


    /// @brief IN条件
    struct CondIn : Builder, Cond {
        FieldLike field_;
        std::vector<ValueLike> values_;

        CondIn(FieldLike field, std::vector<ValueLike> values);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    /// @brief NOT IN条件
    struct CondNotIn : Builder, Cond {
        FieldLike field_;
        std::vector<ValueLike> values_;

        CondNotIn(FieldLike field, std::vector<ValueLike> values);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    /// @brief BETWEEN条件
    struct CondBetween : Builder, Cond {
        FieldLike field_;
        ValueLike start_;
        ValueLike end_;

        CondBetween(FieldLike field, ValueLike start, ValueLike end);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    /// @brief 原生条件
    struct CondRaw : Builder, Cond {
        std::string raw_cond_;

        CondRaw(std::string raw_cond);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    struct CondOp;
    /// @brief 条件连接符类型
    struct CondOpType {
        std::string op_str_;
        /// @brief 连接多个条件
        template<typename... Args>
        std::enable_if_t<std::conjunction_v<std::is_convertible<Args, Condition>...>, CondOp>
        operator()(Args...) const;

        inline bool operator==(const CondOpType &other) const { return op_str_ == other.op_str_; }
        inline bool operator!=(const CondOpType &other) const { return op_str_ != other.op_str_; }
        inline bool operator==(const CondOpType *other) const { return op_str_ == other->op_str_; }
        inline bool operator!=(const CondOpType *other) const { return op_str_ != other->op_str_; }
    };


    static const CondOpType AND = {"AND"};
    static const CondOpType OR = {"OR"};
    static const CondOpType NOT = {"NOT"};


    struct Condition;
    /// @brief 连接条件
    struct CondOp : Builder, Cond {
        const CondOpType *op_;
        std::vector<std::shared_ptr<Condition>> subs_;

        CondOp(const CondOpType &op);
        CondOp(const CondOpType &op, std::vector<std::shared_ptr<Condition>> subs);
        CondOp(const CondOpType &op, std::initializer_list<Condition> subs);
        CondOp &add(Condition c);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    template<typename... Args>
    std::enable_if_t<std::conjunction_v<std::is_convertible<Args, Condition>...>, CondOp>
    CondOpType::operator()(Args... args) const {
        return {*this, std::initializer_list<Condition>{std::forward<Args>(args)...}};
    }


    /// @brief 条件接口
    struct Condition : Builder {
        std::variant<CondOp, CondCmp, CondIn, CondBetween, CondRaw> conditions_;

        Condition(std::variant<CondOp, CondCmp, CondIn, CondBetween, CondRaw> cond);
        Condition(CondOp cond);
        Condition(CondCmp cond);
        Condition(CondIn cond);
        Condition(CondBetween cond);
        Condition(CondRaw cond);
        Condition(const Cond *cond);


        static Condition CAST(const Cond *);

        virtual void build_s(std::ostream &oss, const Type &t = SQLITE) const;
    };


    inline CondOp operator&&(Condition &&left, Condition &&right) { return {AND, {std::move(left), std::move(right)}}; }
    inline CondOp operator||(Condition &&left, Condition &&right) { return {OR, {std::move(left), std::move(right)}}; }
    inline CondOp operator!(Condition &&cond) { return {NOT, {std::move(cond)}}; }

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_COND__HPP_GUARD
