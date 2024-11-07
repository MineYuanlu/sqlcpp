/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_DEFS__HPP_GUARD
#define SQLCPP_DEFS__HPP_GUARD
#include "sqlcpp/types.hpp"
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

/// @namespace sqlcpp
/// @brief sqlcpp所有代码的基础命名空间
namespace sqlcpp {

    static constexpr const auto SQLCPP_DEFAULT_TYPE = SQLITE;
    /// @brief 用于构建SQL语句的基类
    struct Builder {

        /// @brief 分步的将SQL语句输出到流中
        /// @param oss 输出流
        /// @param t 数据库类型，默认为 SQLCPP_DEFAULT_TYPE
        virtual void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const = 0;
        /// @brief 输出SQL语句
        /// @param t 数据库类型，默认为 SQLCPP_DEFAULT_TYPE
        /// @return SQL语句
        std::string build(const Type &t = SQLCPP_DEFAULT_TYPE) const;

        virtual ~Builder() = default;
    };

    struct VarMap;

    /// @brief 参数构建基类
    /// @details 用于探测SQL语句中 IndexedVarValue 的存在, 并组成一个参数构造器
    /// @details 允许用户通过 VarMap 将参数顺序转换为SQL语句所需的顺序
    struct VarBuilder {

        virtual void edit_var_map(VarMap &var_map) const = 0;
        virtual VarMap get_var_map() const;
    };

    /// @brief 参数占位符映射关系管理
    /// @details 记录 SQL 语句中参数('?')的顺序与代码中参数的顺序的映射关系
    /// @details 允许用户通过 bind 将参数顺序转换为SQL语句所需的顺序
    /// @details bind 需要一个能够使用<<运算符传入数据的 binder, 并将参数顺序绑定到 binder 中
    ///
    /// @details 例如:
    /// \code
    /// static auto select = Select("name").from("table").where(Field("age") > VAR(1) && Field("time") < VAR(0));
    /// static auto sql = select.build();
    /// static auto var = select.get_var_map();
    /// CustomBinder binder;
    /// int time = 100;
    /// int age = 18;
    /// std::string ignore;
    /// var.bind(binder, time, age, ignore);
    /// \endcode
    struct VarMap {
        std::vector<size_t> index_sql_to_code;///< index = sql_index, value = code_index

        VarMap() = default;

        /// @brief 由构建语句调用, 记录一个参数占位符的位置
        /// @throw std::invalid_argument 多个占位符指向同一个参数时抛出异常
        void add_var(size_t code_index);

        /// @brief 绑定参数到 binder
        /// @details 绑定参数到 binder, 调用binder的 << 运算符绑定参数, 按照 VarMap 中记录的顺序进行映射绑定
        /// @param binder 绑定器
        /// @param args 参数列表
        /// @tparam Binder 绑定器类型
        /// @tparam Args 参数类型
        /// @throw std::invalid_argument 当参数数量不足时抛出异常
        template<typename Binder, typename... Args>
        void bind(Binder &&binder, Args &&...args) const {
            {
                size_t max = 0;
                for (auto it: index_sql_to_code)
                    if (it > max) max = it;
                if (max >= sizeof...(Args))
                    throw std::invalid_argument("[sqlcpp] VarMap need '" + std::to_string(max + 1) + "' arguments, but only '" + std::to_string(sizeof...(Args)) + "' provided.");
            }
            auto tpl = std::make_tuple((&args)...);
            for (auto it: index_sql_to_code) {
                bind_helper_impl(binder, tpl, it, std::make_index_sequence<sizeof...(Args)>{});
            }
        }

    private:
        // bind 辅助函数实现，将 tuple 中的第 index 个元素通过 << 运算符绑定到 binder
        template<typename Binder, typename Tuple, size_t... Is>
        static void bind_helper_impl(Binder &binder, const Tuple &tpl, size_t index, std::index_sequence<Is...>) {
            // 使用初始化列表和展开表达式，找到匹配的索引并进行绑定
            (void) std::initializer_list<int>{(Is == index ? (binder << std::move(*std::get<Is>(tpl)), 0) : 0)...};
        }
    };
}// namespace sqlcpp
#endif// SQLCPP_DEFS__HPP_GUARD
