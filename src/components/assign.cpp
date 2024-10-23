#include "sqlcpp/components/assign.hpp"
namespace sqlcpp {
    Assign::Assign(FieldLike field, ValueLike value) : field_(std::move(field)), value_(std::move(value)) {}
    Assigns Assign::add(Assign assign) const {
        return std::vector<Assign>{*this, std::move(assign)};
    }
    Assigns Assign::operator+(Assign assign) const { return add(std::move(assign)); }

    void Assign::build_s(std::ostream &oss, const Type &t) const {
        field_.build_s(oss, t);
        oss << " = ";
        value_.build_s(oss, t);
    }


    Assigns::Assigns(FieldLike field, ValueLike value) : Assigns(Assign{std::move(field), std::move(value)}) {}
    Assigns::Assigns(Assign assign) : assigns_({std::move(assign)}) {}
    Assigns::Assigns(std::vector<Assign> assigns) : assigns_(std::move(assigns)) {}
    Assigns::Assigns(std::vector<std::pair<FieldLike, ValueLike>> assigns) {
        assigns_.reserve(assigns.size());
        for (auto &[f, v]: assigns) assigns_.emplace_back(std::move(f), std::move(v));
    }
    Assigns &Assigns::add(Assign assign) {
        assigns_.emplace_back(std::move(assign));
        return *this;
    }
    Assigns &Assigns::operator+=(Assign assign) { return add(std::move(assign)); }
    Assigns &Assigns::operator+=(const Assigns &o) {
        assigns_.insert(assigns_.end(), o.assigns_.begin(), o.assigns_.end());
        return *this;
    }
    Assigns Assigns::operator+(Assign assign) const & { return Assigns(*this).add(std::move(assign)); }
    Assigns &Assigns::operator+(Assign assign) && {
        add(std::move(assign));
        return *this;
    }
    void Assigns::build_s(std::ostream &oss, const Type &t) const {
        for (size_t i = 0; i < assigns_.size(); ++i) {
            if (i > 0) oss << ", ";
            assigns_[i].build_s(oss, t);
        }
    }
}// namespace sqlcpp
