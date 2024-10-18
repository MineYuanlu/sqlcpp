/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_INSERT__HPP_GUARD
#define SQLCPP_COMPONENTS_INSERT__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/table.hpp"
#include "sqlcpp/defs.hpp"
#include <cstddef>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>
namespace sqlcpp {
    /// @brief 插入数据管理器
    /// @details 如果未指定数据, 则使用"?"(VAR)代替
    struct InsertValues {

        InsertValues() = default;
        InsertValues(std::vector<std::vector<ValueLike>> rows);

        /// @brief 添加一行数据
        void add_row(const std::vector<ValueLike> &row);
        void add_row(const std::initializer_list<ValueLike> &row);
        void add_row();
        /// @brief 添加一列数据
        /// @details 如果不同行的列数不一致, 则在每行取最大列数后面继续添加, 缺省的地方将使用"?"(VAR)代替
        void add_col(const std::vector<ValueLike> &col);
        void add_col(const std::initializer_list<ValueLike> &col);

        void build_s(std::ostream &oss, const Type &t, size_t col_num) const;

    private:
        std::vector<std::vector<ValueLike>> rows_{};
        size_t col_num_ = 0;
    };
    struct Insert : Builder {
        enum OperatorModifier {
            LOW_PRIORITY,
            DELAYED,
            HIGH_PRIORITY,
        };
        enum InsertOr {
            OR_ROLLBACK,
            OR_ABORT,
            OR_FAIL,
            OR_IGNORE,///< 等于调用ignore, mysql支持
            OR_REPLACE,
        };

        std::string table_{};
        std::optional<OperatorModifier> op_modifier_{};///< mysql only
        bool IGNORE_ = false;
        std::vector<FieldLike> columns_{};
        std::variant<InsertValues, std::string> values_ = InsertValues{};
        std::optional<InsertOr> INSERT_OR_;                                    ///< sqlite only
        std::optional<std::vector<std::pair<FieldLike, ValueLike>>> DUPLICATE_;///< mysql only
        std::optional<std::vector<FieldLike>> RETURNING_;                      ///< sqlite only

        Insert() = default;
        Insert(const std::string &table) : table_(table) {}
        Insert(const Table &table) : table_(table.table_) {}
        Insert(const From &from) : table_(from.table_name_) {}

        /// @note mysql only
        Insert &op_mod(std::optional<OperatorModifier> om);

        /// @note mysql only
        Insert &ignore(bool v = true);

        ///@note sqlite only
        Insert &insert_or(std::optional<InsertOr> io);

        ///@note mysql only
        Insert &on_duplicate(std::optional<std::vector<std::pair<FieldLike, ValueLike>>> v);

        ///@note mysql only
        template<typename... Args>
        std::enable_if_t<sizeof...(Args) % 2 == 0, Insert &> on_duplicate(Args &&...args);

        Insert &columns(std::vector<FieldLike> cols);
        template<typename... Args>
        Insert &columns(Args &&...args);
        Insert &add_column(FieldLike col);
        Insert &add_columns(const std::vector<FieldLike> &col);
        template<typename... Args>
        Insert &add_columns(Args &&...args);


        /// @brief 添加一行数据 (缺省部分使用"?"(VAR)代替)
        Insert &val(const std::vector<ValueLike> &row);
        /// @brief 添加一行数据 (缺省部分使用"?"(VAR)代替)
        Insert &val(const std::initializer_list<ValueLike> &row);
        /// @brief 添加一行数据 (缺省部分使用"?"(VAR)代替)
        template<typename... Args>
        Insert &val(Args &&...args);
        /// @brief 添加一行"?"(VAR)数据
        Insert &val();

        /// @brief 添加一列数据 (缺省部分使用"?"(VAR)代替)
        Insert &val_col(const std::vector<ValueLike> &col);
        /// @brief 添加一列数据 (缺省部分使用"?"(VAR)代替)
        Insert &val_col(const std::initializer_list<ValueLike> &col);
        /// @brief 添加一列数据 (缺省部分使用"?"(VAR)代替)
        template<typename... Args>
        Insert &val_col(Args &&...args);

        ///@note sqlite only
        Insert &returning(FieldLike r);
        ///@note sqlite only
        Insert &returning(std::optional<std::vector<FieldLike>> r);
        ///@note sqlite only
        template<typename... Args>
        Insert &returning(Args &&...rs);
        ///@note sqlite only
        Insert &add_returning(FieldLike rs);
        ///@note sqlite only
        Insert &add_returning(const std::vector<FieldLike> &rs);
        ///@note sqlite only
        template<typename... Args>
        Insert &add_returning(Args &&...rs);


    private:
        template<typename... Args>
        inline void on_duplicate_impl(std::vector<std::pair<FieldLike, ValueLike>> &all, FieldLike &&k, ValueLike &&v, Args &&...args) {
            all.emplace_back(std::forward<FieldLike>(k), std::forward<ValueLike>(v));
            if constexpr (sizeof...(args) > 0) on_duplicate_impl(all, std::forward<Args>(args)...);
        }

    public:
        void build_s(std::ostream &oss, const Type &t) const override;
    };


    template<typename... Args>
    std::enable_if_t<sizeof...(Args) % 2 == 0, Insert &> Insert::on_duplicate(Args &&...args) {
        std::vector<std::pair<FieldLike, ValueLike>> v;
        v.reserve(sizeof...(Args) / 2);
        on_duplicate_impl(v, std::forward<Args>(args)...);
        return on_duplicate(std::move(v));
    }
    template<typename... Args>
    Insert &Insert::columns(Args &&...args) {
        columns_.clear();
        columns_.reserve(sizeof...(Args));
        (columns_.emplace_back(std::forward<Args>(args)), ...);
        return *this;
    }
    template<typename... Args>
    Insert &Insert::add_columns(Args &&...args) {
        columns_.reserve(columns_.size() + sizeof...(Args));
        (columns_.emplace_back(std::forward<Args>(args)), ...);
        return *this;
    }
    template<typename... Args>
    Insert &Insert::val(Args &&...args) {
        if (auto *val = std::get_if<InsertValues>(&values_); val)
            val->add_row(std::initializer_list<ValueLike>{std::forward<Args>(args)...});
        else
            throw std::invalid_argument("[sqlcpp] Cannot add row after set raw values.");
        return *this;
    }
    template<typename... Args>
    Insert &Insert::val_col(Args &&...args) {
        if (auto *val = std::get_if<InsertValues>(&values_); val)
            val->add_col(std::initializer_list<ValueLike>{std::forward<Args>(args)...});
        else
            throw std::invalid_argument("[sqlcpp] Cannot add row after set raw values.");
        return *this;
    }
    template<typename... Args>
    Insert &Insert::returning(Args &&...rs) {
        RETURNING_.emplace();
        RETURNING_->reserve(sizeof...(Args));
        (RETURNING_->emplace_back(std::forward<Args>(rs)), ...);
        return *this;
    }
    template<typename... Args>
    Insert &Insert::add_returning(Args &&...rs) {
        if (!RETURNING_) RETURNING_ = std::vector<FieldLike>{};
        RETURNING_->reserve(RETURNING_->size() + sizeof...(Args));
        (RETURNING_->emplace_back(std::forward<Args>(rs)), ...);
        return *this;
    }
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_INSERT__HPP_GUARD
