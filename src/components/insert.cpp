#include "sqlcpp/components/insert.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/value.hpp"
#include <optional>
#include <tuple>
#include <variant>
namespace sqlcpp {


    InsertValues::InsertValues(std::vector<std::vector<ValueLike>> rows) : rows_(std::move(rows)) {}


    void InsertValues::add_row(const std::vector<ValueLike> &row) {
        rows_.emplace_back(row);
        col_num_ = std::max(col_num_, row.size());
    }
    void InsertValues::add_row(const std::initializer_list<ValueLike> &row) {
        rows_.emplace_back(row);
        col_num_ = std::max(col_num_, row.size());
    }
    void InsertValues::add_row() {
        rows_.emplace_back();
    }


    void InsertValues::add_col(const std::vector<ValueLike> &col) {
        size_t idx = 0;
        for (auto &field: col) {
            if (idx >= rows_.size()) rows_.emplace_back();
            auto &row = rows_[idx];
            if (row.size() < col_num_) row.resize(col_num_, VAR);
            row.emplace_back(field);
            idx++;
        }
        ++col_num_;
    }
    void InsertValues::add_col(const std::initializer_list<ValueLike> &col) {
        size_t idx = 0;
        for (auto &field: col) {
            if (idx >= rows_.size()) rows_.emplace_back();
            auto &row = rows_[idx];
            if (row.size() < col_num_) row.resize(col_num_, VAR);
            row.emplace_back(field);
            idx++;
        }
        ++col_num_;
    }


    void InsertValues::build_s(std::ostream &oss, const Type &t, size_t col_num) const {
        if (rows_.empty()) throw std::invalid_argument("[sqlcpp] Empty insert values.");
        if (col_num_ > col_num) throw std::invalid_argument("[sqlcpp] Invalid column number: " + std::to_string(col_num_) + " > " + std::to_string(col_num));
        oss << "VALUES ";
        for (size_t i = 0; i < rows_.size(); ++i) {
            if (i > 0) oss << ',';
            oss << '(';
            auto &row = rows_[i];
            for (size_t j = 0; j < col_num; ++j) {
                if (j > 0) oss << ',';
                if (j < row.size()) {
                    row[j].build_s(oss, t);
                } else {
                    VAR.build_s(oss, t);
                }
            }
            oss << ')';
        }
    }


    Insert::Insert(std::string table) : table_(std::move(table)) {}
    Insert::Insert(const char *table) : table_(std::move(table)) {}
    Insert::Insert(const Table &table) : table_(table.table_) {}
    Insert::Insert(const From &from) : table_(from.table_name_) {}


    Insert &Insert::op_mod(std::optional<OperatorModifier> om) {
        op_modifier_ = std::move(om);
        return *this;
    }


    Insert &Insert::ignore(bool v) {
        if (v)
            INSERT_OR_ = OR_IGNORE;
        else
            INSERT_OR_ = std::nullopt;
        return *this;
    }


    Insert &Insert::insert_or(std::optional<InsertOr> io) {
        INSERT_OR_ = std::move(io);
        return *this;
    }

    Insert &Insert::on_duplicate(std::optional<std::vector<std::pair<FieldLike, ValueLike>>> v) {
        if (v) DUPLICATE_ = std::make_tuple(std::nullopt, std::move(*v));
        else
            DUPLICATE_ = {};
        return *this;
    }
    Insert &Insert::on_duplicate(Assigns a) {
        DUPLICATE_ = std::make_tuple(std::nullopt, std::move(a));
        return *this;
    }
    Insert &Insert::on_duplicate(Assign a) {
        DUPLICATE_ = std::make_tuple(std::nullopt, std::move(a));
        return *this;
    }


    void Insert::build_s(std::ostream &oss, const Type &t) const {
        oss << "INSERT ";
        if (t == MYSQL) {
            if (op_modifier_) {
                // clang-format off
                switch (*op_modifier_) {
                    case LOW_PRIORITY: oss << "LOW_PRIORITY "; break;
                    case DELAYED:      oss << "DELAYED "; break;
                    case HIGH_PRIORITY:oss << "HIGH_PRIORITY "; break;
                }
                // clang-format on
            }
        } else if (t == SQLITE) {
            if (INSERT_OR_) {
                oss << "OR ";
                // clang-format off
                switch (*INSERT_OR_) {
                    case OR_ROLLBACK: oss << "ROLLBACK "; break;
                    case OR_ABORT:    oss << "ABORT "; break;
                    case OR_FAIL:     oss << "FAIL "; break;
                    case OR_IGNORE:   /* 稍后输出 */ break;
                    case OR_REPLACE:  oss << "REPLACE "; break;
                }
                // clang-format on
            }
        } else {
            throw std::invalid_argument("[sqlcpp] Unknown SQL type: " + std::string{t});
        }
        if (INSERT_OR_ == OR_IGNORE) oss << "IGNORE ";
        oss << "INTO " << table_;

        oss << '(';
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ',';
            columns_[i].build_s(oss, t);
        }
        oss << ')' << ' ';

        if (auto *val = std::get_if<InsertValues>(&values_); val) {
            val->build_s(oss, t, columns_.size());
        } else if (auto *raw = std::get_if<std::string>(&values_); raw) {
            oss << *raw;
        } else {
            throw std::invalid_argument("[sqlcpp] Invalid values type for insert statement.");
        }

        if (t == MYSQL) {
            const auto &[notMYSQL, assigns] = DUPLICATE_;
            if (!notMYSQL && assigns) {
                oss << " ON DUPLICATE KEY UPDATE ";
                assigns->build_s(oss, t);
            }
        } else if (t == SQLITE) {
            const auto &[conflict, assigns] = DUPLICATE_;
            if (conflict) {
                oss << " ON CONFLICT(";
                for (size_t i = 0; i < conflict->size(); ++i) {
                    if (i > 0) oss << ", ";
                    conflict->at(i).build_s(oss, t);
                }
                oss << ") DO ";
                if (assigns) {
                    oss << "UPDATE SET ";
                    assigns->build_s(oss, t);
                } else {
                    oss << "NOTHING";
                }
            }
            if (RETURNING_) {
                oss << " RETURNING ";
                auto &returing = *RETURNING_;
                for (size_t i = 0; i < returing.size(); ++i) {
                    if (i > 0) oss << ", ";
                    returing[i].build_s(oss, t);
                }
            }
        }
        oss << ';';
    }


    Insert &Insert::columns(std::vector<FieldLike> cols) {
        columns_ = std::move(cols);
        return *this;
    }
    Insert &Insert::add_column(FieldLike col) {
        columns_.emplace_back(std::move(col));
        return *this;
    }
    Insert &Insert::add_columns(const std::vector<FieldLike> &col) {
        columns_.insert(columns_.end(), col.begin(), col.end());
        return *this;
    }
    Insert &Insert::val(const std::vector<ValueLike> &row) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) val->add_row(row);
        else
            throw std::invalid_argument("[sqlcpp] Cannot add row after set raw values.");
        return *this;
    }
    Insert &Insert::val(const std::initializer_list<ValueLike> &row) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) val->add_row(row);
        else
            throw std::invalid_argument("[sqlcpp] Cannot add row after set raw values.");
        return *this;
    }
    Insert &Insert::val() {
        if (auto *val = std::get_if<InsertValues>(&values_); val) val->add_row();
        else
            throw std::invalid_argument("[sqlcpp] Cannot add row after set raw values.");
        return *this;
    }
    Insert &Insert::val_col(const std::vector<ValueLike> &col) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) val->add_col(col);
        else
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    Insert &Insert::val_col(const std::initializer_list<ValueLike> &col) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) val->add_col(col);
        else
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    Insert &Insert::key_value(FieldLike field, const std::vector<ValueLike> &values) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) {
            if (val->col_num() != columns_.size()) {
                throw std::invalid_argument("[sqlcpp] column name and value size not match: " + std::to_string(val->col_num()) + " vs " + std::to_string(columns_.size()));
            }
            add_column(std::move(field));
            val->add_col(values);
        } else
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    Insert &Insert::key_value(FieldLike field, const std::initializer_list<ValueLike> &values) {
        if (auto *val = std::get_if<InsertValues>(&values_); val) {
            if (val->col_num() != columns_.size()) {
                throw std::invalid_argument("[sqlcpp] column name and value size not match: " + std::to_string(val->col_num()) + " vs " + std::to_string(columns_.size()));
            }
            add_column(std::move(field));
            val->add_col(values);
        } else
            throw std::invalid_argument("[sqlcpp] Cannot add col after set raw values.");
        return *this;
    }
    Insert &Insert::on_conflict(FieldLike field, std::vector<std::pair<FieldLike, ValueLike>> set) {
        DUPLICATE_ = std::make_tuple(std::vector<FieldLike>{std::move(field)}, std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(FieldLike field, Assigns set) {
        DUPLICATE_ = std::make_tuple(std::vector<FieldLike>{std::move(field)}, std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(FieldLike field, Assign set) {
        DUPLICATE_ = std::make_tuple(std::vector<FieldLike>{std::move(field)}, std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(FieldLike field, [[maybe_unused]] Conflict) {
        DUPLICATE_ = std::make_tuple(std::vector<FieldLike>{std::move(field)}, std::nullopt);
        return *this;
    }
    Insert &Insert::on_conflict(std::vector<FieldLike> fields, std::vector<std::pair<FieldLike, ValueLike>> set) {
        DUPLICATE_ = std::make_tuple(std::move(fields), std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(std::vector<FieldLike> fields, Assigns set) {
        DUPLICATE_ = std::make_tuple(std::move(fields), std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(std::vector<FieldLike> fields, Assign set) {
        DUPLICATE_ = std::make_tuple(std::move(fields), std::move(set));
        return *this;
    }
    Insert &Insert::on_conflict(std::vector<FieldLike> fields, [[maybe_unused]] Conflict) {
        DUPLICATE_ = std::make_tuple(std::move(fields), std::nullopt);
        return *this;
    }
    Insert &Insert::returning(FieldLike r) {
        RETURNING_ = std::vector<FieldLike>{r};
        return *this;
    }
    Insert &Insert::returning(std::optional<std::vector<FieldLike>> r) {
        RETURNING_ = std::move(r);
        return *this;
    }
    Insert &Insert::add_returning(FieldLike rs) {
        if (!RETURNING_) RETURNING_ = std::vector<FieldLike>{};
        RETURNING_->emplace_back(rs);
        return *this;
    }
    Insert &Insert::add_returning(const std::vector<FieldLike> &rs) {
        if (!RETURNING_) RETURNING_ = rs;
        else
            RETURNING_->insert(RETURNING_->end(), rs.begin(), rs.end());
        return *this;
    }
}// namespace sqlcpp
