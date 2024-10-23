/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_INSERT__HPP_GUARD
#define SQLCPP_COMPONENTS_INSERT__HPP_GUARD
#include "sqlcpp/components/assign.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/table.hpp"
#include "sqlcpp/defs.hpp"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <variant>
#include <vector>
namespace sqlcpp {
    /// @brief 插入数据管理器
    /// @details 如果未指定数据, 则使用"?"(VAR)代替
    struct InsertValues final {

        /// @brief 默认构造
        InsertValues() = default;
        /// @brief 使用指定数据构造 row[ col[ value ] ]
        InsertValues(std::vector<std::vector<ValueLike>> rows);

        /// @brief 添加一行数据
        void add_row(const std::vector<ValueLike> &row);
        /// @brief 添加一行数据
        void add_row(const std::initializer_list<ValueLike> &row);
        /// @brief 添加一行数据
        /// @details 此行所有数据均为"?"(VAR)
        void add_row();
        /// @brief 添加一列数据
        /// @details 如果不同行的列数不一致, 则在每行取最大列数后面继续添加, 缺省的地方将使用"?"(VAR)代替
        void add_col(const std::vector<ValueLike> &col);
        /// @brief 添加一列数据
        /// @details 如果不同行的列数不一致, 则在每行取最大列数后面继续添加, 缺省的地方将使用"?"(VAR)代替
        void add_col(const std::initializer_list<ValueLike> &col);

        /// @brief 构建sql组件
        /// @param col_num 需要的列数, 如果数据列大于需要的列数, 则抛出异常
        /// @throw std::invalid_argument
        void build_s(std::ostream &oss, const Type &t, size_t col_num) const;

        /// @brief 列数(取所有行中最大的)
        [[nodiscard]] size_t col_num() const noexcept { return col_num_; }
        /// @brief 行数
        [[nodiscard]] size_t row_num() const noexcept { return rows_.size(); }

    private:
        std::vector<std::vector<ValueLike>> rows_{};
        size_t col_num_ = 0;
    };
    /// @brief 插入语句构建器
    struct Insert final : public Builder {
        /// @brief 操作优先级
        /// @note mysql only
        enum OperatorModifier {
            LOW_PRIORITY = 1,///< 不会打断或阻塞正在进行的读操作
            DELAYED,         ///< 延迟插入, 仅MyISAM
            HIGH_PRIORITY,   ///< 即使有正在进行的读操作，插入操作也会立即执行，优先处理
        };
        ///@brief 插入失败时的处理方式
        ///@note sqlite only
        enum InsertOr {
            OR_FAIL,    ///< 默认行为，遇到冲突时插入失败
            OR_ROLLBACK,///< 回滚所有操作
            OR_ABORT,   ///< 回滚当前事务，插入操作失败
            OR_IGNORE,  ///< 忽略该插入操作; 等于调用ignore, mysql支持
            OR_REPLACE, ///< 删除原行并插入新行
        };

        /// @brief 冲突处理方式
        ///@note sqlite only
        enum Conflict {
            DO_NOTHING,
        };

        std::string table_{};                          ///< 插入的表名
        std::optional<OperatorModifier> op_modifier_{};///< mysql only
        std::vector<FieldLike> columns_{};             ///< 插入的列名
        /// @brief 插入的数据
        /// @details string代表raw sql
        std::variant<InsertValues, std::string> values_ = InsertValues{};
        std::optional<InsertOr> INSERT_OR_{};///< sqlite only
        ///mysql: ON DUPLICATE KEY UPDATE ; sqlite: ON CONFLICT(field, ...) DO
        ///@note 是否有field区分mysql/sqlite
        ///@note sqlite >= 3.24.0
        std::tuple<std::optional<std::vector<FieldLike>>, std::optional<Assigns>> DUPLICATE_{};
        std::optional<std::vector<FieldLike>> RETURNING_{};///< sqlite AND mysql >= 8.0.19

        ///@brief 默认构造
        Insert() = default;
        /// @brief 指定表名构造
        Insert(std::string table);
        /// @brief 指定表名构造
        Insert(const char *table);
        /// @brief 指定表名构造
        Insert(const Table &table);
        /// @brief 指定表名构造
        Insert(const From &from);

        /// @brief 指定操作优先级
        /// @note mysql only
        Insert &op_mod(std::optional<OperatorModifier> om);

        /// @brief 指定插入失败时跳过
        /// @note mysql only
        /// @note 等于insert_or(OR_IGNORE)
        Insert &ignore(bool v = true);

        /// @brief 插入失败时的操作
        ///@note sqlite only
        Insert &insert_or(std::optional<InsertOr> io);

        /// @brief KEY重复时插入的数据
        ///@note mysql only
        Insert &on_duplicate(std::optional<std::vector<std::pair<FieldLike, ValueLike>>> a);
        /// @brief KEY重复时插入的数据
        ///@note mysql only
        Insert &on_duplicate(Assigns a);
        /// @brief KEY重复时插入的数据
        ///@note mysql only
        Insert &on_duplicate(Assign a);
        /// @brief KEY重复时插入的数据
        ///@note mysql only
        template<typename... Args>
        Insert &on_duplicate(Args &&...args) {
            DUPLICATE_ = std::make_tuple(std::nullopt, Assigns{std::forward<Args>(args)...});
            return *this;
        }

        /// @brief 设置插入的列名
        Insert &columns(std::vector<FieldLike> cols);
        /// @brief 设置插入的列名
        template<typename... Args>
        Insert &columns(Args &&...args);
        /// @brief 添加一个插入的列名
        Insert &add_column(FieldLike col);
        /// @brief 添加一些插入的列名
        Insert &add_columns(const std::vector<FieldLike> &col);
        /// @brief 添加一些插入的列名
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

        /// @brief 添加一列数据, 不会添加列名称 (缺省部分使用"?"(VAR)代替)
        Insert &val_col(const std::vector<ValueLike> &col);
        /// @brief 添加一列数据, 不会添加列名称 (缺省部分使用"?"(VAR)代替)
        Insert &val_col(const std::initializer_list<ValueLike> &col);
        /// @brief 添加一列数据, 不会添加列名称 (缺省部分使用"?"(VAR)代替)
        template<typename... Args>
        Insert &val_col(Args &&...args);

        /// @brief 添加一个新列名, 同时插入这列数据 (缺省部分使用"?"(VAR)代替)
        Insert &key_value(FieldLike field, const std::vector<ValueLike> &values);
        /// @brief 添加一个新列名, 同时插入这列数据 (缺省部分使用"?"(VAR)代替)
        Insert &key_value(FieldLike field, const std::initializer_list<ValueLike> &values);
        /// @brief 添加一个新列名, 同时插入这列数据 (缺省部分使用"?"(VAR)代替)
        template<typename... Args>
        Insert &key_value(FieldLike field, Args &&...args);

        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(FieldLike field, std::vector<std::pair<FieldLike, ValueLike>> set);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(FieldLike field, Assigns set);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(FieldLike field, Assign set);
        /// @brief field 冲突时做指定操作
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(FieldLike field, Conflict DO);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        template<typename... Args>
        Insert &on_conflict(FieldLike field, Args &&...set);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(std::vector<FieldLike> fields, std::vector<std::pair<FieldLike, ValueLike>> set);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(std::vector<FieldLike> fields, Assigns set);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(std::vector<FieldLike> fields, Assign set);
        /// @brief field 冲突时做指定操作
        ///@note sqlite >= 3.24.0
        Insert &on_conflict(std::vector<FieldLike> fields, Conflict DO);
        /// @brief field 冲突时插入数据
        ///@note sqlite >= 3.24.0
        template<typename... Args>
        Insert &on_conflict(std::vector<FieldLike> fields, Args &&...set);


        /// @brief 插入后返回指定列数据
        ///@note sqlite AND mysql >= 8.0.19
        Insert &returning(FieldLike r);
        /// @brief 插入后返回指定列数据
        ///@note sqlite AND mysql >= 8.0.19
        Insert &returning(std::optional<std::vector<FieldLike>> r);
        ///@note sqlite AND mysql >= 8.0.19
        /// @brief 插入后返回指定列数据
        template<typename... Args>
        Insert &returning(Args &&...rs);
        /// @brief 添加一个插入后返回的列
        ///@note sqlite AND mysql >= 8.0.19
        Insert &add_returning(FieldLike rs);
        /// @brief 添加一些插入后返回的列
        ///@note sqlite AND mysql >= 8.0.19
        Insert &add_returning(const std::vector<FieldLike> &rs);
        /// @brief 添加一些插入后返回的列
        ///@note sqlite AND mysql >= 8.0.19
        template<typename... Args>
        Insert &add_returning(Args &&...rs);


    private:
        template<typename... Args>
        inline void on_duplicate_impl(std::vector<std::pair<FieldLike, ValueLike>> &all, FieldLike &&k, ValueLike &&v, Args &&...args) {
            all.emplace_back(std::forward<FieldLike>(k), std::forward<ValueLike>(v));
            if constexpr (sizeof...(args) > 0) on_duplicate_impl(all, std::forward<Args>(args)...);
        }

    public:
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
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
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    template<typename... Args>
    Insert &Insert::key_value(FieldLike field, Args &&...args) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) {
            if (val->col_num() != columns_.size()) {
                throw std::invalid_argument("[sqlcpp] column name and value size not match: " + std::to_string(val->col_num()) + " vs " + std::to_string(columns_.size()));
            }
            add_column(std::move(field));
            val->add_col(std::initializer_list<ValueLike>{std::forward<Args>(args)...});
        } else
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    template<typename... Args>
    Insert &Insert::on_conflict(FieldLike field, Args &&...set) {
        DUPLICATE_ = std::make_tuple(std::vector<FieldLike>{std::move(field)}, Assigns{std::forward<Args>(set)...});
        return *this;
    }
    template<typename... Args>
    Insert &Insert::on_conflict(std::vector<FieldLike> fields, Args &&...set) {
        DUPLICATE_ = std::make_tuple(std::move(fields), Assigns{std::forward<Args>(set)...});
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
