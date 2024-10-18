#include "sqlcpp/components/insert.hpp"
#include "sqlcpp/components/value.hpp"
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


    Insert &Insert::op_mod(std::optional<OperatorModifier> om) {
        op_modifier_ = std::move(om);
        return *this;
    }


    Insert &Insert::ignore(bool v) {
        IGNORE_ = v;
        return *this;
    }


    Insert &Insert::insert_or(std::optional<InsertOr> io) {
        INSERT_OR_ = std::move(io);
        return *this;
    }


    Insert &Insert::on_duplicate(std::optional<std::vector<std::pair<FieldLike, ValueLike>>> v) {
        DUPLICATE_ = std::move(v);
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
        if (IGNORE_ || INSERT_OR_ == OR_IGNORE) oss << "IGNORE ";
        oss << "INTO " << table_;

        oss << '(';
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ',';
            columns_[i].build_s(oss, t);
        }
        oss << ')' << ' ';

        if (auto *val = std::get_if<InsertValues>(&values_); val) {
            val->build_s(oss, t, columns_.size());
        } else if (auto *val = std::get_if<std::string>(&values_); val) {
            oss << *val;
        } else {
            throw std::invalid_argument("[sqlcpp] Invalid values type for insert statement.");
        }

        if (t == MYSQL) {
            if (DUPLICATE_) {
                oss << " ON DUPLICATE KEY UPDATE ";
                bool more = false;
                for (const auto &[k, v]: *DUPLICATE_) {
                    if (more) oss << ",";
                    else
                        more = true;
                    k.build_s(oss, t);
                    oss << " = ";
                    v.build_s(oss, t);
                }
            }
        } else if (t == SQLITE) {
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
