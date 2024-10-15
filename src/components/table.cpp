#include "sqlcpp/components/table.hpp"
#include "sqlcpp/components/column.hpp"
#include "sqlcpp/str.hpp"
#include <algorithm>
#include <set>
#include <variant>
namespace sqlcpp {

    using namespace sqlcpp::str;

    Table::Table(std::string table) : table_(std::move(table)) {}

    Table &Table::content(const Content &c) {
        content_.emplace_back(c);
        return *this;
    }
    Table &Table::if_exists(IfExistsAction action) {
        if_exists_ = action;
        return *this;
    }
    Table &Table::engine(const std::optional<std::string> &eng) {
        engine_ = eng;
        return *this;
    }
    Table &Table::charset(const std::optional<std::string> &ch) {
        charset_ = ch;
        return *this;
    }
    Table &Table::collate(const std::optional<std::string> &col) {
        collate_ = col;
        return *this;
    }
    Table &Table::index(Index index) {
        index.table(table_);
        index_.emplace_back(std::move(index));
        return *this;
    }

    namespace {
        /// @brief 生成给定对象的
        template<class T, class Cmp>
        std::enable_if_t<std::is_invocable_r_v<bool, Cmp, const T &, const T &>, std::vector<size_t>>
        generateSortedIndex(const std::vector<T> &vec, const Cmp &cmp) {
            // Create an index vector with 0, 1, 2, ..., vec.size()-1
            std::vector<size_t> indices(vec.size());
            for (size_t i = 0; i < vec.size(); ++i) indices[i] = i;
            // Sort the index vector based on the values in vec
            std::sort(indices.begin(), indices.end(), [&vec, &cmp](size_t a, size_t b) { return cmp(vec[a], vec[b]); });
            return indices;
        }
        int getContentOrderIndex(const Table::Content &v) {
            return std::visit([](const auto &value) -> int {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, Column>)
                    return 0;
                else if constexpr (std::is_same_v<T, PrimaryKey>)
                    return 1;
                else if constexpr (std::is_same_v<T, Unique>)
                    return 2;
                else if constexpr (std::is_same_v<T, ForeignKey>)
                    return 3;
                else if constexpr (std::is_same_v<T, Check>)
                    return 4;
                else
                    return 5;// Should never be hit if all types are covered
            },
                              v);
        };
    }// namespace

    void Table::build_s(std::ostream &oss, const Type &t) const {
        oss << "CREATE TABLE ";

        if (if_exists_ == DROP) {
            oss << "IF EXISTS ";
        } else if (if_exists_ == SKIP) {
            oss << "IF NOT EXISTS ";
        }

        oss << safe_table(table_, t) << " (\n";


        std::optional<std::vector<std::string>> primary_key;
        auto idx = generateSortedIndex(content_, [](const Content &a, const Content &b) {
            return getContentOrderIndex(a) < getContentOrderIndex(b);
        });
        for (size_t i = 0; i < idx.size(); ++i) {
            auto &content = content_[idx[i]];
            std::visit([&oss, &t](auto &&arg) { oss << ' '; arg.build_s(oss, t); }, content);
            if (i < idx.size() - 1) oss << ",";
            oss << "\n";
            if (std::holds_alternative<Column>(content) && std::get<Column>(content).PRIMARY_KEY_) {
                if (primary_key) throw std::runtime_error("[sqlcpp] Table '" + table_ + "' has duplicate primary keys defined");
                primary_key.emplace(std::vector<std::string>{std::get<Column>(content).name_});
            }
            if (std::holds_alternative<PrimaryKey>(content)) {
                if (primary_key) throw std::runtime_error("[sqlcpp] Table '" + table_ + "' has duplicate primary keys defined");
                primary_key.emplace(std::get<PrimaryKey>(content).columns_);
            }
        }
        bool hasFTS = false;
        if (t == MYSQL) {//full text search
            for (size_t i = 0; i < idx.size(); ++i) {
                auto &content = content_[idx[i]];
                if (!std::holds_alternative<Column>(content)) continue;
                const auto &field = std::get<Column>(content);
                if (!field.FULL_TEXT_SEARCH_) continue;
                oss << ", FULLTEXT INDEX " << safe_field(field.name_ + "_fts", t) << " (" << safe_field(field.name_, t) << ")\n";
                hasFTS = true;
            }
        }

        oss << ')';

        if (t == MYSQL && engine_) {
            std::set<std::string_view> fts_engine{"innodb", "myisam"};
            if (!hasFTS && fts_engine.count(str::toLowerCase(*engine_)) <= 0)
                throw std::runtime_error("[sqlcpp] Table '" + table_ + "' need a valid engine for full text search, not '" + *engine_ + '\'');
            oss << " ENGINE=" << *engine_;
        }

        if (t == MYSQL && charset_) {
            oss << " DEFAULT CHARSET=" << *charset_;
        }

        if (t == MYSQL && collate_) {
            oss << " COLLATE=" << *collate_;
        }

        oss << ";";

        for (const Index &idx: index_) {
            oss << '\t' << '\n';
            idx.build_s(oss, t);
        }

        if (t == SQLITE) {//full text search
            for (size_t i = 0; i < idx.size(); ++i) {
                auto &content = content_[idx[i]];
                if (!std::holds_alternative<Column>(content)) continue;
                const auto &f = std::get<Column>(content);
                if (!f.FULL_TEXT_SEARCH_) continue;
                if (!primary_key || primary_key->size() != 1) throw std::runtime_error("[sqlcpp] Table '" + table_ + "' need a single primary key");
                const auto vt = table_ + "_" + f.name_ + "_fts";
                const auto table = safe_table(table_, t);
                const auto field = safe_field(f.name_, t);
                const auto pk = safe_field(primary_key->at(0), t);
                oss << "\t\nCREATE VIRTUAL TABLE " << safe_table(vt, t) << " USING fts5(" << field << ", content_rowid = " << pk << ");";

                oss << "\t\nCREATE TRIGGER " << safe_table(vt + "_ai", t) << " AFTER INSERT ON " << table << " BEGIN ";
                oss << "INSERT INTO " << safe_table(vt, t) << "(rowid," << field << ") VALUES (new." << pk << ", new." << field << ");";
                oss << "END;";

                oss << "\t\nCREATE TRIGGER " << safe_table(vt + "_au", t) << " AFTER UPDATE ON " << table << " BEGIN ";
                oss << "UPDATE " << safe_table(vt, t) << " SET " << field << " = new." << field << " WHERE rowid = new." << pk << ";";
                oss << "END;";

                oss << "\t\nCREATE TRIGGER " << safe_table(vt + "_ad", t) << " AFTER DELETE ON " << table << " BEGIN ";
                oss << "DELETE FROM " << safe_table(vt, t) << " WHERE rowid = old." << pk << ";";
                oss << "END;";


                hasFTS = true;
            }
        }
    }
}// namespace sqlcpp
