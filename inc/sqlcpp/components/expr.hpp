#ifndef xx_SQLCPP_COMPONENTS_EXPR_HPP_
#define xx_SQLCPP_COMPONENTS_EXPR_HPP_

#include "sqlcpp/defs.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
namespace sqlcpp {
    struct OpExpr;
    struct CaseExpr;
    struct FuncExpr;
    struct ExprLike;
    struct FieldLike;
    struct Field;
    struct RawField;
    struct ValueLike;
    struct Value;
    struct RawValue;
    struct NullValue;
    struct IndexedVarValue;
    struct VarValue;
    struct BlobValue;
    struct Select;
    struct Condition;


    /// @brief 表达式基类
    /// @details 代表一个通用的表达式, 可以是基本的字段、字面量、变量, 也可以是函数、运算表达式、case、子查询等复杂表达式
    struct Expr final : public Builder, public VarBuilder {
        using InnerValue = std::variant<
                std::shared_ptr<ExprLike>,
                std::shared_ptr<FieldLike>,
                std::shared_ptr<ValueLike>,
                std::shared_ptr<Select>
                //
                >;
        InnerValue value_;///< 数据代理

        Expr(InnerValue v);                                           ///< 直接构造
        Expr(OpExpr expr);                                            ///< 运算表达式构造
        Expr(CaseExpr expr);                                          ///< 条件表达式构造
        Expr(FuncExpr expr);                                          ///< 函数表达式构造
        Expr(ExprLike expr);                                          ///< 表达式构造
        Expr(FieldLike field);                                        ///< 字段构造
        Expr(Field field);                                            ///< 字段构造
        Expr(std::string table, std::string field);                   ///< 字段构造
        Expr(std::string table, std::string field, std::string alias);///< 字段构造
        Expr(RawField field);                                         ///< 字段构造
        Expr(ValueLike value);                                        ///< 字面量构造
        Expr(Value value);                                            ///< 字面量构造
        Expr(RawValue value);                                         ///< 字面量构造
        Expr(NullValue value);                                        ///< 字面量构造
        Expr(IndexedVarValue value);                                  ///< 占位符构造
        Expr(VarValue value);                                         ///< 占位符构造
        Expr(BlobValue value);                                        ///< 字面量构造
        Expr(const char *value);                                      ///< 字面量构造
        Expr(std::string value);                                      ///< 字面量构造
        Expr(int64_t value);                                          ///< 字面量构造
        Expr(uint64_t value);                                         ///< 字面量构造
        Expr(double value);                                           ///< 字面量构造
        Expr(float value);                                            ///< 字面量构造
        Expr(bool value);                                             ///< 字面量构造
        /// @brief 对于任意宽度的有符号整数, 构造Expr时会自动转换为int64_t
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type * = nullptr>
        Expr(T value) : Expr(static_cast<int64_t>(value)) {}
        /// @brief 对于任意宽度的无符号整数, 构造Expr时会自动转换为uint64_t
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type * = nullptr>
        Expr(T value) : Expr(static_cast<uint64_t>(value)) {}
        Expr(std::nullptr_t);///< 字面量构造
        Expr(std::nullopt_t);///< 字面量构造
        Expr(Select select); ///< 子查询构造

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

    /// @brief 运算表达式
    /// @details 代表一个运算关系, 如A + B, A - B, A * B, A / B, A % B
    struct OpExpr final : public Builder, public VarBuilder {
        enum Op {
            ADD,///< A + B
            SUB,///< A - B
            MUL,///< A * B
            DIV,///< A / B
            MOD,///< A % B
        };
        /// @brief 构造运算表达式
        static constexpr const char *op_name(Op op) {
            switch (op) {
                    // clang-format off
                case ADD: return " + ";
                case SUB: return " - ";
                case MUL: return " * ";
                case DIV: return " / ";
                case MOD: return " % ";
                // clang-format on
                default:
                    throw std::invalid_argument("[sqlcpp::OpExpr] Invalid operator: " + std::to_string(int(op)));
            }
        }

        Expr a;///< 左操作数
        Expr b;///< 右操作数
        Op op; ///< 运算符

        std::optional<std::string> alias_;///< 别名
        OpExpr(Op op, Expr a, Expr b);

        OpExpr &alias(std::string alias);///< 设置别名
        OpExpr &as(std::string alias);   ///< 设置别名

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

    template<typename L, typename R, typename = std::enable_if_t<std::is_convertible_v<L, Expr> && std::is_convertible_v<R, Expr> && (std::is_same_v<std::decay_t<L>, Expr> || std::is_same_v<std::decay_t<R>, Expr>)>>
    inline OpExpr operator+(L &&left, R &&right) { return {OpExpr::ADD, std::forward<L>(left), std::forward<R>(right)}; }
    template<typename L, typename R, typename = std::enable_if_t<std::is_convertible_v<L, Expr> && std::is_convertible_v<R, Expr> && (std::is_same_v<std::decay_t<L>, Expr> || std::is_same_v<std::decay_t<R>, Expr>)>>
    inline OpExpr operator-(L &&left, R &&right) { return {OpExpr::SUB, std::forward<L>(left), std::forward<R>(right)}; }
    template<typename L, typename R, typename = std::enable_if_t<std::is_convertible_v<L, Expr> && std::is_convertible_v<R, Expr> && (std::is_same_v<std::decay_t<L>, Expr> || std::is_same_v<std::decay_t<R>, Expr>)>>
    inline OpExpr operator*(L &&left, R &&right) { return {OpExpr::MUL, std::forward<L>(left), std::forward<R>(right)}; }
    template<typename L, typename R, typename = std::enable_if_t<std::is_convertible_v<L, Expr> && std::is_convertible_v<R, Expr> && (std::is_same_v<std::decay_t<L>, Expr> || std::is_same_v<std::decay_t<R>, Expr>)>>
    inline OpExpr operator/(L &&left, R &&right) { return {OpExpr::DIV, std::forward<L>(left), std::forward<R>(right)}; }
    template<typename L, typename R, typename = std::enable_if_t<std::is_convertible_v<L, Expr> && std::is_convertible_v<R, Expr> && (std::is_same_v<std::decay_t<L>, Expr> || std::is_same_v<std::decay_t<R>, Expr>)>>
    inline OpExpr operator%(L &&left, R &&right) { return {OpExpr::MOD, std::forward<L>(left), std::forward<R>(right)}; }


    /// @brief 条件表达式
    /// @details 同时支持"简单CASE"和"搜索CASE"模式
    /// @details CASE [expr] WHEN <cond1|check1> THEN <val1> [WHEN <cond2|check2> THEN <val2>...] [ELSE <default_val>] END
    /// @example CASE WHEN s > 0 THEN 'big' WHEN s < 0 THEN 'small' ELSE 'zero' END
    /// @example CASE field WHEN 'active' THEN 1 WHEN 'inactive' THEN 0 END
    struct CaseExpr : public Builder, public VarBuilder {
        std::optional<Expr> expr_;                                                          ///< switch值
        std::vector<std::pair<std::variant<std::shared_ptr<Condition>, Expr>, Expr>> cases_;///< case值: 条件 - 值
        std::optional<Expr> else_;                                                          ///< default值

        std::optional<std::string> alias_;///< 别名

        /// @brief 构造条件表达式
        /// @details 接受任意 expr, [check1, val1], ..., [checkN, valN], [default] 作为参数, 如:
        /// @code
        /// CaseExpr(expr)
        /// CaseExpr(expr, check1, val1, default)
        /// CaseExpr(expr, check1, val1, check2, val2, default)
        /// CaseExpr(expr, default)
        /// @endcode
        template<typename... Args>
        explicit CaseExpr(Expr expr, Args... args) : expr_(std::move(expr)) {
            if constexpr (sizeof...(args) >= 2) {
                add_cases(std::forward<Args>(args)...);
            } else if constexpr (sizeof...(args) == 1) {
                set_else(std::forward<Args>(args)...);
            }
        }
        /// @brief 构造条件表达式
        /// @details 接受任意 [cond1, val1], ..., [condN, valN], [default] 作为参数, 如:
        /// @code
        /// CaseExpr()
        /// CaseExpr(cond1, val1, default)
        /// CaseExpr(cond1, val1, cond2, val2, default)
        /// CaseExpr(default)
        /// @endcode
        template<typename... Args>//TODO
        explicit CaseExpr(Args... args) {
            if constexpr (sizeof...(args) >= 2) {
                add_cases(std::forward<Args>(args)...);
            } else if constexpr (sizeof...(args) == 1) {
                set_else(std::forward<Args>(args)...);
            }
        }

        /// @brief 设置被switch的值
        CaseExpr &set_expr(Expr expr);
        /// @brief 添加case
        CaseExpr &add_case(Condition cond, Expr expr);
        /// @brief 添加case
        CaseExpr &add_case(Expr cond, Expr expr);
        /// @brief 设置default值
        CaseExpr &set_else(Expr expr);

        /// @brief 添加一组case
        /// @details 接受任意 cond|check, expr, ..., cond|check, expr, [default] 作为参数
        template<typename C, typename = std::enable_if_t<std::is_convertible_v<C, Condition> || std::is_convertible_v<C, Expr>>,
                 typename E, typename = std::enable_if_t<std::is_convertible_v<E, Expr>>,
                 typename... Args>
        CaseExpr &add_cases(C &&cond, E &&expr, Args &&...args) {
            add_case(std::forward<C>(cond), std::forward<E>(expr));
            if constexpr (sizeof...(args) >= 2) {
                add_cases(std::forward<Args>(args)...);
            } else if constexpr (sizeof...(args) == 1) {
                set_else(std::forward<Args>(args)...);
            }
            return *this;
        }


        CaseExpr &alias(std::string alias);///< 设置别名
        CaseExpr &as(std::string alias);   ///< 设置别名

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

    /// @brief 函数表达式
    /// @details 代表一个函数调用, 如: ABS(B), LENGTH(C), MAX(D, E), COUNT(*)
    struct FuncExpr final : public Builder, public VarBuilder {
        std::string name_;      ///< 函数名
        std::vector<Expr> args_;///< 参数列表

        std::optional<std::string> alias_;///< 别名

        FuncExpr(std::string name, std::vector<Expr> args) : name_(std::move(name)), args_(std::move(args)) {}
        template<typename... Args, typename = std::enable_if_t<(std::is_convertible_v<Args, Expr> && ...)>>
        FuncExpr(std::string name, Args &&...args) : name_(std::move(name)) {
            args_.reserve(sizeof...(args));
            (args_.emplace_back(std::forward<Args>(args)), ...);
        }

        FuncExpr &set_name(std::string name);///< 设置函数名

        FuncExpr &add_arg(Expr arg);///< 添加参数

        template<typename... Args, typename = std::enable_if_t<std::conjunction_v<std::is_convertible<Args, Expr>...>>>
        FuncExpr &add_args(Args &&...args) {
            (add_arg(std::forward<Args>(args)), ...);
            return *this;
        }

        FuncExpr &alias(std::string alias);///< 设置别名
        FuncExpr &as(std::string alias);   ///< 设置别名

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


    /// @brief 基本表达式
    /// @details OpExpr, CaseExpr, FuncExpr 被算作基本表达式, 使用 ExprLike 来包装。
    /// @details 而 ExprLike, FieldLike, ValueLike, Select 等不被算作基本表达式, 使用 Expr 来包装
    struct ExprLike : public Builder, public VarBuilder {
        std::variant<OpExpr, CaseExpr, FuncExpr> expr_;///< 基本表达式

        ExprLike(OpExpr expr);  ///< 运算表达式构造
        ExprLike(CaseExpr expr);///< 条件表达式构造
        ExprLike(FuncExpr expr);///< 函数表达式构造

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const;
        void edit_var_map(VarMap &var_map) const;
    };

}// namespace sqlcpp
#endif// xx_SQLCPP_COMPONENTS_EXPR_HPP_