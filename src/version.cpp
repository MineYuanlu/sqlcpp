#include "sqlcpp/version.hpp"
namespace sqlcpp {
    int version::major() {
#ifdef SQLCPP_VERSION_MAJOR
        return SQLCPP_VERSION_MAJOR;
#else
        return 0;
#endif
    }
    int version::minor() {
#ifdef SQLCPP_VERSION_MINOR
        return SQLCPP_VERSION_MINOR;
#else
        return 0;
#endif
    }
    int version::patch() {
#ifdef SQLCPP_VERSION_PATCH
        return SQLCPP_VERSION_PATCH;
#else
        return 0;
#endif
    }
    const char *version::special() {
#ifdef SQLCPP_VERSION_SPECIAL
        return SQLCPP_VERSION_SPECIAL;
#else
        return nullptr;
#endif
    }
    const char *version::string() {
#ifdef SQLCPP_VERSION_STRING
        return SQLCPP_VERSION_STRING;
#else
        return nullptr;
#endif
    }
}// namespace sqlcpp