#include "sqlcpp/components/index.hpp"
#include "sqlcpp/str.hpp"
namespace sqlcpp {

    using namespace sqlcpp::str;

    Index::Index(std::string field, Order order) {
        this->field(std::move(field), std::move(order));
    }
    Index::Index(std::string table, std::string field, Order order) {
        this->table(std::move(table));
        this->field(std::move(field), std::move(order));
    }
    Index &Index::table(std::optional<std::string> table) {
        table_ = std::move(table);
        return *this;
    }
    Index &Index::name(std::string name) {
        name_ = std::move(name);
        user_set_name = true;
        return *this;
    }
    Index &Index::unique(bool unique) {
        unique_ = unique;
        return *this;
    }
    Index &Index::field(std::string field, Order order) {
        if (!user_set_name) {
            if (!name_.empty()) name_ += '_';
            name_ += field;
        }
        fields_.emplace_back(std::move(field), std::move(order));
        return *this;
    }
    Index &Index::fields(const std::vector<std::tuple<std::string, Order>> &fs) {
        for (const auto &[f, o]: fs) this->field(f, o);
        return *this;
    }


    void Index::build_s(std::ostream &oss, const Type &t) const {
        auto name = user_set_name ? name_ : (name_ + "_idx");
        if (!table_) throw std::runtime_error("[sqlcpp] Index '" + name + "' not set table");
        oss << "CREATE ";
        if (unique_) oss << "UNIQUE ";
        oss << "INDEX " << safe_field(name, t);
        oss << " ON " << safe_table(*table_, t) << " (\n";
        for (size_t i = 0; i < fields_.size(); ++i) {
            if (i > 0) oss << ",\n";
            oss << "  " << safe_field(std::get<0>(fields_[i]), t) << " ";
            oss << order_str(std::get<1>(fields_[i]));
        }
        oss << "\n);";
    }
}// namespace sqlcpp
