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


    struct Table final : public Builder {
        enum IfExistsAction {
            NONE,
            DROP,
            SKIP,
        };

        std::string table_{};
        using Content = std::variant<Column, PrimaryKey, Unique, ForeignKey, Check>;
        std::vector<Content> content_{};
        std::vector<Index> index_{};
        IfExistsAction if_exists_{};
        std::optional<std::string> engine_ = "InnoDB";
        std::optional<std::string> charset_ = "utf8mb4";
        std::optional<std::string> collate_{};

        Table(std::string table);

        Table &content(const std::vector<Content> &c);
        Table &content(const Content &c);

        Table &if_exists(IfExistsAction action);

        Table &engine(const std::optional<std::string> &eng);

        Table &charset(const std::optional<std::string> &ch);

        Table &collate(const std::optional<std::string> &col);

        Table &index(Index index);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_TABLE__HPP_GUARD
