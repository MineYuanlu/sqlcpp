#include "sqlcpp/components/update.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/types.hpp"
#include <cstddef>
#include <stdexcept>
#include <variant>
namespace sqlcpp {
    Update::Update(Assigns assigns)
        : assigns_(std::move(assigns)) {}

    Update::Update(const char *table, std::optional<Assigns> assigns)
        : table_(From{std::move(table)}), assigns_(std::move(assigns)) {}

    Update::Update(std::string table, std::optional<Assigns> assigns)
        : table_(From{std::move(table)}), assigns_(std::move(assigns)) {}

    Update::Update(From table, std::optional<Assigns> assigns)
        : table_(std::move(table)), assigns_(std::move(assigns)) {}

    Update::Update(RawFrom table, std::optional<Assigns> assigns)
        : table_(std::move(table)), assigns_(std::move(assigns)) {}

    Update::Update(Froms table, std::optional<Assigns> assigns)
        : table_(std::move(table)), assigns_(std::move(assigns)) {}

    Update::Update(const char *table, FieldLike field, ValueLike value)
        : table_(From{std::move(table)}), assigns_(Assign{std::move(field), std::move(value)}) {}

    Update::Update(std::string table, FieldLike field, ValueLike value)
        : table_(From{std::move(table)}), assigns_(Assign{std::move(field), std::move(value)}) {}

    Update::Update(From table, FieldLike field, ValueLike value)
        : table_(std::move(table)), assigns_(Assign{std::move(field), std::move(value)}) {}

    Update::Update(RawFrom table, FieldLike field, ValueLike value)
        : table_(std::move(table)), assigns_(Assign{std::move(field), std::move(value)}) {}

    Update::Update(Froms table, FieldLike field, ValueLike value)
        : table_(std::move(table)), assigns_(Assign{std::move(field), std::move(value)}) {}


    Update &Update::low_priority(bool v) {
        LOW_PRIORITY = v;
        return *this;
    }
    Update &Update::ignore(bool v) {
        IGNORE = v;
        return *this;
    }
    Update &Update::update_or(std::optional<UpdateOr> v) {
        UPDATE_OR_ = std::move(v);
        return *this;
    }
    Update &Update::table(std::string table) {
        table_ = From{std::move(table)};
        return *this;
    }
    Update &Update::table(From table) {
        table_ = std::move(table);
        return *this;
    }
    Update &Update::table(RawFrom table) {
        table_ = std::move(table);
        return *this;
    }
    Update &Update::table(Froms table) {
        table_ = std::move(table);
        return *this;
    }
    Update &Update::assign(Assign assign) {
        assigns_.emplace(std::move(assign));
        return *this;
    }
    Update &Update::assign(FieldLike field, ValueLike value) {
        assigns_.emplace(std::move(field), std::move(value));
        return *this;
    }
    Update &Update::assign(Assigns assigns) {
        assigns_ = std::move(assigns);
        return *this;
    }
    Update &Update::add_assign(Assign assign) {
        if (!assigns_) assigns_.emplace(std::move(assign));
        else
            *assigns_ += std::move(assign);
        return *this;
    }
    Update &Update::add_assign(FieldLike field, ValueLike value) {
        if (!assigns_) assigns_.emplace(std::move(field), std::move(value));
        else
            *assigns_ += Assign{std::move(field), std::move(value)};
        return *this;
    }
    Update &Update::add_assign(const Assigns &assigns) {
        if (!assigns_) assigns_.emplace(std::move(assigns));
        else
            *assigns_ += std::move(assigns);
        return *this;
    }
    Update &Update::where(Where where) {
        where_ = std::move(where);
        return *this;
    }
    Update &Update::where(Condition where) {
        where_.emplace(std::move(where));
        return *this;
    }
    Update &Update::order_by(OrderBy order_by) {
        order_by_ = std::move(order_by);
        return *this;
    }
    Update &Update::order_by(Field field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Update &Update::order_by(RawField field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Update &Update::order_by(FuncField field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Update &Update::order_by(FieldLike field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Update &Update::order_by(OrderByField field) {
        order_by_.emplace(std::move(field));
        return *this;
    }
    Update &Update::limit(size_t limit) {
        limit_ = limit;
        return *this;
    }
    Update &Update::offset(size_t offset) {
        offset_ = offset;
        return *this;
    }
    Update &Update::limit(const VarValue &limit) {
        limit_ = limit;
        return *this;
    }
    Update &Update::offset(const VarValue &offset) {
        offset_ = offset;
        return *this;
    }
    Update &Update::returning(FieldLike r) {
        RETURNING_ = std::vector<FieldLike>{r};
        return *this;
    }
    Update &Update::returning(std::optional<std::vector<FieldLike>> r) {
        RETURNING_ = std::move(r);
        return *this;
    }
    Update &Update::add_returning(FieldLike rs) {
        if (!RETURNING_) RETURNING_ = std::vector<FieldLike>{};
        RETURNING_->emplace_back(rs);
        return *this;
    }
    Update &Update::add_returning(const std::vector<FieldLike> &rs) {
        if (!RETURNING_) RETURNING_ = rs;
        else
            RETURNING_->insert(RETURNING_->end(), rs.begin(), rs.end());
        return *this;
    }

    void Update::build_s(std::ostream &oss, const Type &t) const {
        if (!assigns_) throw std::invalid_argument("[sqlcpp] Update assigns is empty");
        if (!table_) throw std::invalid_argument("[sqlcpp] Update table is empty");
        if (offset_ && t != SQLITE) throw std::invalid_argument("[sqlcpp] Update offset is only supported by sqlite");

        oss << "UPDATE ";
        if (t == MYSQL && LOW_PRIORITY) oss << "LOW_PRIORITY ";
        if (t == MYSQL && IGNORE) oss << "IGNORE ";


        table_->build_s(oss, t);


        oss << " SET ";
        assigns_->build_s(oss, t);

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
        if (RETURNING_) {
            oss << " RETURNING ";
            auto &returing = *RETURNING_;
            for (size_t i = 0; i < returing.size(); ++i) {
                if (i > 0) oss << ", ";
                returing[i].build_s(oss, t);
            }
        }
        oss << ';';
    }

    // MYSQL
    // UPDATE [LOW_PRIORITY] [IGNORE] <table_reference>
    // SET <column1> = {<expr1>|DEFAULT} [, <column2> = {<expr2>|DEFAULT}]...
    // [WHERE <where_condition>]
    // [ORDER BY <order_expression>]
    // [LIMIT <row_count>]
    //
    // [LOW_PRIORITY]：可选，降低UPDATE语句的优先级。
    // [IGNORE]：可选，忽略更新过程中出现的错误。
    // <table_reference>：必选，可以是单个表名或多表更新的表列表。
    // SET：必选，指定要更新的列和新值。
    // [WHERE <where_condition>]：可选，指定更新哪些行。
    // [ORDER BY <order_expression>]：可选，指定更新行的顺序。
    // [LIMIT <row_count>]：可选，限制被更新的行数。


    // SQLITE
    // UPDATE [OR <ROLLBACK|ABORT|FAIL|IGNORE|REPLACE>] <table_name>
    // SET <column1> = <expr1> [, <column2> = <expr2>]...
    // [FROM <from_clause>]
    // [WHERE <expression>]
    // [ORDER BY <order_expression>]
    // [LIMIT <count>]
    // [RETURNING <expression_list>]
    //
    // [OR <ROLLBACK|ABORT|FAIL|IGNORE|REPLACE>]：可选，指定冲突解决算法。
    // <table_name>：必选，指定要更新的表。
    // SET：必选，指定要更新的列和新值。
    // [FROM <from_clause>]：可选，允许在UPDATE中使用其他表，类似于JOIN操作。
    // [WHERE <expression>]：可选，指定更新哪些行。
    // [ORDER BY <order_expression>]：可选，指定更新行的顺序。
    // [LIMIT <count>]：可选，限制被更新的行数。
    // [RETURNING <expression_list>]：可选，返回每个被更新行的指定表达式。
}// namespace sqlcpp
