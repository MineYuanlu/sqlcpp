#include "sqlcpp/components/where.hpp"
namespace sqlcpp {
    Where::Where(Condition condition) : condition_(std::move(condition)) {}
    void Where::build_s(std::ostream &oss, const Type &t) const {
        oss << " WHERE ";
        condition_.build_s(oss, t);
    }
}// namespace sqlcpp
