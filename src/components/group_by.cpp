#include "sqlcpp/components/group_by.hpp"
namespace sqlcpp {
    GroupBy::GroupBy(FieldLike field) : fields_({std::move(field)}) {}
    GroupBy::GroupBy(std::vector<FieldLike> fields) : fields_(std::move(fields)) {}
    GroupBy &GroupBy::by(FieldLike field) {
        fields_.emplace_back(std::move(field));
        return *this;
    }
    void GroupBy::build_s(std::ostream &oss, const Type &t) const {
        if (fields_.empty()) return;
        oss << " GROUP BY ";
        const auto sz = fields_.size();
        for (size_t i = 0; i < sz; ++i) {
            fields_[i].build_s(oss, t);
            if (i < sz - 1) oss << ", ";
        }
    }
}// namespace sqlcpp
