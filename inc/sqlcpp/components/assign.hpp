/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
#define SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/value.hpp"
#include "sqlcpp/defs.hpp"
namespace sqlcpp {
    struct Assigns;

    /// @brief 赋值组件, 代表对一个字段的赋值操作
    struct Assign final : public Builder {
        FieldLike field_;///< 被赋值的字段
        ValueLike value_;///< 赋值的值

        Assign(FieldLike field, ValueLike value = VAR);

        /// @brief 与另一个赋值合并为一个赋值组
        Assigns add(Assign assign) const;
        /// @brief 与另一个赋值合并为一个赋值组
        Assigns operator+(Assign assign) const;


        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    /// @brief 赋值组组件, 代表一组赋值操作
    struct Assigns final : public Builder {
        std::vector<Assign> assigns_;///< 赋值组

        /// @brief 空赋值组
        Assigns() = default;
        /// @brief 用一个赋值初始化赋值组
        Assigns(FieldLike field, ValueLike value = VAR);
        /// @brief 用一个赋值初始化赋值组
        Assigns(Assign assign);
        /// @brief 用一组赋值组初始化赋值组
        Assigns(std::vector<Assign> assigns);
        /// @brief 用一组赋值组初始化赋值组
        Assigns(std::vector<std::pair<FieldLike, ValueLike>> assigns);
        /// @brief 用一组赋值组初始化赋值组
        template<typename... Args>
        Assigns(Args... args) {
            assigns_.reserve(sizeof...(args));
            (assigns_.emplace_back(std::forward<Args>(args)), ...);
        }


        /// @brief 添加一个赋值到赋值组
        Assigns &add(Assign assign);

        Assigns &operator+=(Assign assign);
        Assigns &operator+=(const Assigns &assign);
        Assigns operator+(Assign assign) const &;
        Assigns &operator+(Assign assign) &&;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_ASSIGN__HPP_GUARD
