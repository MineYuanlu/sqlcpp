/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_TABLE__HPP_GUARD
#define SQLCPP_COMPONENTS_TABLE__HPP_GUARD
#include "sqlcpp/components/check.hpp"
#include "sqlcpp/components/column.hpp"
#include "sqlcpp/components/index.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <variant>
#include <vector>
namespace sqlcpp {


    /// @brief 表构建器
    struct Table final : public Builder {
        /// @brief 表存在时执行的动作
        enum IfExistsAction {
            NONE,///< 不执行任何操作
            DROP,///< 删除表
            SKIP,///< 跳过创建表操作
        };

        using Content = std::variant<Column, PrimaryKey, Unique, ForeignKey, Check>;///< 表内容


        std::string table_{};                           ///< 表名
        std::vector<Content> content_{};                ///< 表内容
        std::vector<Index> index_{};                    ///< 索引
        IfExistsAction if_exists_{};                    ///< 表存在时执行的动作
        std::optional<std::string> engine_ = "InnoDB";  ///< 引擎 mysql only
        std::optional<std::string> charset_ = "utf8mb4";///< 字符集 mysql only
        std::optional<std::string> collate_{};          ///< 排序规则 mysql only

        /// @brief 构造函数
        /// @param table 表名
        Table(std::string table);

        /// @brief 添加一些表内容
        /// @details 支持: 列 / 主键 / 唯一约束 / 外键 / 约束
        Table &content(const std::vector<Content> &c);
        /// @brief 添加一个表内容
        /// @details 支持: 列 / 主键 / 唯一约束 / 外键 / 约束
        Table &content(const Content &c);

        /// @brief 设置表存在时执行的动作
        Table &if_exists(IfExistsAction action);

        /// @brief 设置引擎
        Table &engine(const std::optional<std::string> &eng);

        /// @brief 设置字符集
        Table &charset(const std::optional<std::string> &ch);

        /// @brief 设置排序规则
        Table &collate(const std::optional<std::string> &col);

        /// @brief 添加一个索引
        Table &index(Index index);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_TABLE__HPP_GUARD
