#include "sqlcpp/components/where.hpp"
namespace sqlcpp {
    Where::Where(Condition condition) : condition_(std::move(condition)) {}
    void Where::build_s(std::ostream &oss, const Type &t) const {
        oss << " WHERE ";
        condition_.build_s(oss, t);
    }
    void Where::edit_var_map(VarMap &var_map) const {
        condition_.edit_var_map(var_map);
    }
}// namespace sqlcpp
