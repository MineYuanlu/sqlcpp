#include "sqlcpp/components/check.hpp"
namespace sqlcpp {
    Check::Check(std::string check) : check_(std::move(check)) {}


    void Check::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << "CHECK (" << check_ << ')';
    }

}// namespace sqlcpp
