#include "sqlcpp/components/select.hpp"
#include "sqlcpp/components/value.hpp"
namespace sqlcpp {
    Select::Select(FieldLike field) : fields_({std::move(field)}) {}
    Select::Select(std::vector<FieldLike> fields) : fields_(std::move(fields)) {}

    Select &Select::select(const char *field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::select(std::string field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::select(FieldLike field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::select(Field field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::select(RawField field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::select(Expr field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    Select &Select::from(Froms from) {
        from_ = std::move(from);
        return *this;
    }
    Select &Select::from(From from) {
        from_.emplace(std::move(from));
        return *this;
    }
    Select &Select::from(RawFrom from) {
        from_.emplace(std::move(from));
        return *this;
    }
    Select &Select::where(Where where) {
        where_ = std::move(where);
        return *this;
    }
    Select &Select::where(Condition where) {
        where_.emplace(std::move(where));
        return *this;
    }
    Select &Select::group_by(GroupBy group_by) {
        group_by_ = std::move(group_by);
        return *this;
    }
    Select &Select::having(Having having) {
        having_ = std::move(having);
        return *this;
    }
    Select &Select::having(Condition having) {
        having_.emplace(std::move(having));
        return *this;
    }
    Select &Select::order_by(OrderBy order_by) {
        order_by_ = std::move(order_by);
        return *this;
    }
    Select &Select::order_by(Field field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Select &Select::order_by(RawField field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Select &Select::order_by(Expr field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Select &Select::order_by(FieldLike field, Order o) {
        order_by_.emplace(std::move(field), std::move(o));
        return *this;
    }
    Select &Select::order_by(OrderByField field) {
        order_by_.emplace(std::move(field));
        return *this;
    }
    Select &Select::limit(size_t limit) {
        limit_ = limit;
        return *this;
    }
    Select &Select::offset(size_t offset) {
        offset_ = offset;
        return *this;
    }
    Select &Select::limit(const VarValue &limit) {
        limit_ = limit;
        return *this;
    }
    Select &Select::offset(const VarValue &offset) {
        offset_ = offset;
        return *this;
    }
    Select &Select::limit(const IndexedVarValue &limit) {
        limit_ = limit;
        return *this;
    }
    Select &Select::offset(const IndexedVarValue &offset) {
        offset_ = offset;
        return *this;
    }

    void Select::build_s(std::ostream &oss, const Type &t) const {
        build_s(oss, t, false);
    }
    void Select::build_s(std::ostream &oss, const Type &t, bool is_subquery) const {
        oss << "SELECT ";
        const auto sz = fields_.size();
        for (size_t i = 0; i < sz; ++i) {
            fields_[i].build_s(oss, t);
            if (i < sz - 1) oss << ", ";
        }
        if (from_) {
            oss << " FROM ";
            from_->build_s(oss, t);
        }
        if (where_) where_->build_s(oss, t);
        if (group_by_) group_by_->build_s(oss, t);
        if (having_) having_->build_s(oss, t);
        if (order_by_) order_by_->build_s(oss, t);
        if (limit_) {
            oss << " LIMIT ";
            if (auto val = std::get_if<size_t>(&*limit_); val) {
                oss << *val;
            } else if (auto var = std::get_if<VarValue>(&*limit_); var) {
                var->build_s(oss, t);
            } else if (auto idx = std::get_if<IndexedVarValue>(&*limit_); idx) {
                idx->build_s(oss, t);
            }
        }
        if (offset_) {
            oss << " OFFSET ";
            if (auto val = std::get_if<size_t>(&*offset_); val) {
                oss << *val;
            } else if (auto var = std::get_if<VarValue>(&*offset_); var) {
                var->build_s(oss, t);
            } else if (auto idx = std::get_if<IndexedVarValue>(&*offset_); idx) {
                idx->build_s(oss, t);
            }
        }
        if (!is_subquery) oss << ';';
    }
    void Select::edit_var_map(VarMap &var_map) const {
        if (where_) where_->edit_var_map(var_map);
        if (group_by_) group_by_->edit_var_map(var_map);
        if (having_) having_->edit_var_map(var_map);
        if (order_by_) order_by_->edit_var_map(var_map);
        if (limit_) {
            if (auto var = std::get_if<VarValue>(&*limit_); var) {
                var->edit_var_map(var_map);
            } else if (auto idx = std::get_if<IndexedVarValue>(&*limit_); idx) {
                idx->edit_var_map(var_map);
            }
        }
        if (offset_) {
            if (auto var = std::get_if<VarValue>(&*offset_); var) {
                var->edit_var_map(var_map);
            } else if (auto idx = std::get_if<IndexedVarValue>(&*offset_); idx) {
                idx->edit_var_map(var_map);
            }
        }
    }
}// namespace sqlcpp
