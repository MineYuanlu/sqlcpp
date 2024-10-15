#include "sqlcpp/components/having.hpp"
namespace sqlcpp {
    Having::Having(Condition condition) : condition_(std::move(condition)) {}
    void Having::build_s(std::ostream &oss, const Type &t) const {
        oss << " HAVING ";
        condition_.build_s(oss, t);
    }
}// namespace sqlcpp
