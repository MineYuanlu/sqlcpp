#include "sqlcpp/defs.hpp"
#include <sstream>

namespace sqlcpp {

    std::string Builder::build(const Type &t) const {
        std::ostringstream oss;
        build_s(oss, t);
        return oss.str();
    }
    VarMap VarBuilder::get_var_map() const {
        VarMap var_map;
        edit_var_map(var_map);
        return var_map;
    }

    void VarMap::add_var(size_t code_index) {
        for (auto &i: index_sql_to_code) {
            if (i == code_index) {
                throw std::invalid_argument("[sqlcpp] VarMap received duplicate variable index: " + std::to_string(code_index));
            }
        }
        index_sql_to_code.emplace_back(code_index);
    }
}// namespace sqlcpp