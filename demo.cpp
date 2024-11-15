
#include <memory>
#include <utility>
class DemoB;

class DemoA {
    std::shared_ptr<DemoB> b;

public:
    DemoA(DemoB b);
};

class DemoB {
    DemoA a;

public:
    // DemoB(DemoA a) : a(a) {}
    template<typename... Args,
             typename = std::enable_if_t<(
                     (
                             std::is_convertible_v<Args, DemoA>) &&...)>>
    DemoB(Args &&...args) : a(std::forward<Args>(args)...) {}
};

DemoA::DemoA(DemoB b) : b(std::make_shared<DemoB>(b)) {}


int x(DemoA a1, DemoA a2) {
    auto x = std::make_pair(a1, a2);
    return 0;
}