#include "sqlcpp/components/delete.hpp"
#include <stdexcept>
#include <variant>
namespace sqlcpp {
    Delete::Delete(const char *table, std::optional<Where> where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(std::string table, std::optional<Where> where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(From table, std::optional<Where> where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(RawFrom table, std::optional<Where> where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(Froms table, std::optional<Where> where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(const char *table, Condition where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(std::string table, Condition where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(From table, Condition where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(RawFrom table, Condition where) : table_(std::move(table)), where_(std::move(where)) {}
    Delete::Delete(Froms table, Condition where) : table_(std::move(table)), where_(std::move(where)) {}

    Delete &Delete::low_priority(bool v) {
        LOW_PRIORITY = v;
        return *this;
    }
    Delete &Delete::quick(bool v) {
        QUICK = v;
        return *this;
    }
    Delete &Delete::ignore(bool v) {
        IGNORE = v;
        return *this;
    }
    Delete &Delete::from(const char *table) {
        table_.emplace(std::move(table));
        return *this;
    }
    Delete &Delete::from(std::string table) {
        table_.emplace(std::move(table));
        return *this;
    }
    Delete &Delete::from(From table) {
        table_.emplace(std::move(table));
        return *this;
    }
    Delete &Delete::from(RawFrom table) {
        table_.emplace(std::move(table));
        return *this;
    }
    Delete &Delete::from(Froms table) {
        table_.emplace(std::move(table));
        return *this;
    }
    Delete &Delete::where(Condition where) {
        where_.emplace(std::move(where));
        return *this;
    }
    Delete &Delete::where(Where where) {
        where_.emplace(std::move(where));
        return *this;
    }
    Delete &Delete::order_by(OrderBy order_by) {
        order_by_.emplace(std::move(order_by));
        return *this;
    }
    Delete &Delete::order_by(Field field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Delete &Delete::order_by(RawField field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Delete &Delete::order_by(FuncField field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Delete &Delete::order_by(FieldLike field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Delete &Delete::order_by(OrderByField field) {
        order_by_.emplace(std::move(field));
        return *this;
    }
    Delete &Delete::limit(size_t limit) {
        limit_.emplace(limit);
        return *this;
    }
    Delete &Delete::offset(size_t offset) {
        limit_.emplace(offset);
        return *this;
    }
    Delete &Delete::limit(const VarValue &limit) {
        limit_.emplace(limit);
        return *this;
    }
    Delete &Delete::offset(const VarValue &offset) {
        limit_.emplace(offset);
        return *this;
    }
    void Delete::build_s(std::ostream &oss, const Type &t) const {
        if (!table_) throw std::invalid_argument("[sqlcpp] delete table is not set");
        oss << "DELETE ";
        if (t == MYSQL && LOW_PRIORITY) oss << "LOW_PRIORITY ";
        if (t == MYSQL && QUICK) oss << "QUICK ";
        if (t == MYSQL && IGNORE) oss << "IGNORE ";
        oss << "FROM ";
        table_->build_s(oss, t);
        if (where_) where_->build_s(oss, t);
        if (order_by_) order_by_->build_s(oss, t);
        if (limit_) {
            oss << " LIMIT ";
            if (auto val = std::get_if<size_t>(&*limit_); val) {
                oss << *val;
            } else if (auto var = std::get_if<VarValue>(&*limit_); var) {
                var->build_s(oss, t);
            }
        }
        if (offset_) {
            oss << " OFFSET ";
            if (auto val = std::get_if<size_t>(&*offset_); val) {
                oss << *val;
            } else if (auto var = std::get_if<VarValue>(&*offset_); var) {
                var->build_s(oss, t);
            }
        }
    }
}// namespace sqlcpp
