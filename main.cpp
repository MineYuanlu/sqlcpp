#include "sqlcpp/components/select.hpp"
#include <iostream>
/// @brief test
int main() {
    using namespace sqlcpp;

    From p("project", "p");
    From u("user", "u");
    From t("task", "t");

    Select select(p["id"].as("pid"), u["name"].as("owner_name"), t["name"].as("task_name"));
    select.from(p
                        .join(LEFT_JOIN, u, p["owner"] == u["id"])
                        .join(LEFT_JOIN, t, p["id"] == t["project_id"]))
            .where(t["active"] == true && t["user"] == u["id"]);
    auto sql = select.build();
    std::cout << sql << std::endl;
    return 0;
}