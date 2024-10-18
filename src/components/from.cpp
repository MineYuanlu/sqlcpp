#include "sqlcpp/components/from.hpp"
#include "sqlcpp/str.hpp"
namespace sqlcpp {

    using namespace sqlcpp::str;

    From::From(const char *table_name) : table_name_(std::move(table_name)), alias_(std::nullopt) {}
    From::From(std::string table_name) : table_name_(std::move(table_name)), alias_(std::nullopt) {}
    From::From(std::string table_name, std::optional<std::string> alias) : table_name_(std::move(table_name)), alias_(std::move(alias)) {}
    From &From::alias(std::string alias) {
        alias_ = std::move(alias);
        return *this;
    }
    Field From::operator[](std::string field) const {
        if (alias_) return Field(*alias_, std::move(field));
        return Field(table_name_, std::move(field));
    }
    Froms From::join(JoinType type, From from, Condition condition) const {
        return Froms(*this).join(std::move(type), std::move(from), std::move(condition));
    }
    Froms From::join(JoinType type, RawFrom from, Condition condition) const {
        return Froms(*this).join(std::move(type), std::move(from), std::move(condition));
    }

    void From::build_s(std::ostream &oss, const Type &t) const {
        oss << safe_table(table_name_, t);
        if (alias_) oss << " AS " << safe_table(*alias_, t);
    }

    RawFrom::RawFrom(std::string raw_from) : raw_from_(std::move(raw_from)) {}
    void RawFrom::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << raw_from_;
    }

    Froms::Froms(From from) : from_(std::move(from)) {}
    Froms::Froms(RawFrom from) : from_(std::move(from)) {}
    Froms &Froms::join(JoinType type, From from, Condition condition) {
        joins_.emplace_back(type, std::move(from), std::move(condition));
        return *this;
    }
    Froms &Froms::join(JoinType type, RawFrom from, Condition condition) {
        joins_.emplace_back(type, std::move(from), std::move(condition));
        return *this;
    }
    void Froms::build_s(std::ostream &oss, const Type &t) const {
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, from_);
        for (const auto &join: joins_) {
            oss << ' ' << join_type_str(std::get<0>(join)) << ' ';
            std::visit([&](const auto &arg) { arg.build_s(oss, t); }, std::get<1>(join));
            oss << " ON ";
            std::get<2>(join).build_s(oss, t);
        }
    }
}// namespace sqlcpp
