#include "sqlcpp/components/funcs.hpp"
namespace sqlcpp {
    FuncField F::NamedFunc::operator()(Field arg) const {
        return FuncField(name_, arg);
    }
    FuncField F::NamedFunc::operator()(RawField arg) const {
        return FuncField(name_, arg);
    }
}// namespace sqlcpp
