#include "sqlcpp/components/column.hpp"
#include "sqlcpp/components/delete.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/components/from.hpp"
#include "sqlcpp/components/insert.hpp"
#include "sqlcpp/components/select.hpp"
#include "sqlcpp/components/table.hpp"
#include "sqlcpp/components/update.hpp"
#include "sqlcpp/components/value.hpp"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sqlite3.h>
#include <stdexcept>
#include <sys/types.h>

#define ASSERT_SQLITE_A(expr, expect, code, msg_summoner, OR, finish)                                                                                              \
    do {                                                                                                                                                           \
        char *err_msg = nullptr;                                                                                                                                   \
        (void *) err_msg;                                                                                                                                          \
        auto rc = expr;                                                                                                                                            \
        if ((rc != expect) && (OR)) {                                                                                                                              \
            std::cerr << "SQLITE ERROR: " << msg_summoner << ", code: " << rc << ", expect: " << expect << ", LINE: " << __LINE__ << ", expr: " code << std::endl; \
            throw std::runtime_error("SQLITE ERROR");                                                                                                              \
        }                                                                                                                                                          \
        finish;                                                                                                                                                    \
    } while (0)
#define ASSERT_SQLITE(expr, expect) ASSERT_SQLITE_C(expr, expect, #expr, sqlite3_errmsg(db))
#define ASSERT_SQLITE_D(expr, expect, msg_summoner) ASSERT_SQLITE_C(expr, expect, #expr, msg_summoner)
#define ASSERT_SQLITE_C(expr, expect, code, msg_summoner) ASSERT_SQLITE_A(expr, expect, code, msg_summoner, true, )

struct Database;
struct Stmt {
    ~Stmt() {
        if (stmt) {
            sqlite3_finalize(stmt);
            std::cout << "============= FINISH [" << uuid << "] =============" << std::endl;
        }
    }
    Stmt(const Stmt &) = delete;
    Stmt &operator=(const Stmt &) = delete;
    Stmt(Stmt &&other) noexcept : stmt(other.stmt) { other.stmt = nullptr; }
    Stmt &operator=(Stmt &&other) noexcept {
        if (this != &other) {
            if (stmt) sqlite3_finalize(stmt);
            stmt = other.stmt;
            uuid = other.uuid;
            other.stmt = nullptr;
        }
        return *this;
    }
    sqlite3_stmt *stmt;

private:
    friend struct Database;
    uint64_t uuid;
    Stmt(sqlite3_stmt *stmt, uint64_t uuid) : stmt(stmt), uuid(uuid) {}
};
struct Database {
    sqlite3 *db;
    Database() {
        ASSERT_SQLITE(sqlite3_open(":memory:", &db), SQLITE_OK);
    }
    ~Database() {
        sqlite3_close(db);
    }
    void exec(std::string const &sql) {
        std::cout << std::endl
                  << "============= EXEC SQL =============" << std::endl
                  << sql << std::endl;
        ASSERT_SQLITE_D(sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg), SQLITE_OK, sqlite3_errmsg(db) << ", msg: " << err_msg);
    }


    Stmt prepare(std::string const &sql) {
        static uint64_t UUID = 0;
        auto uuid = UUID++;
        std::cout << std::endl
                  << "============= PREPARE SQL [" << uuid << "] =============" << std::endl
                  << sql << std::endl;
        sqlite3_stmt *stmt;
        ASSERT_SQLITE(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0), SQLITE_OK);
        return {stmt, uuid};
    }
    template<typename Func, typename... Args>
    void bind(Func func, const Stmt &stmt, int index, Args... args) {
        ASSERT_SQLITE_C(func(stmt.stmt, index, args...), SQLITE_OK, "BIND DATA", sqlite3_errmsg(db));
    }
    /// @return has next row
    bool run(const Stmt &stmt) {
        int ret;
        ASSERT_SQLITE_A(sqlite3_step(stmt.stmt), SQLITE_DONE, "RUN STEP", sqlite3_errmsg(db), rc != SQLITE_ROW, ret = rc);
        return ret == SQLITE_ROW;
    }
    void reset(const Stmt &stmt) {
        ASSERT_SQLITE(sqlite3_reset(stmt.stmt), SQLITE_OK);
    }

    auto row_id() const { return sqlite3_last_insert_rowid(db); }


    void run_select(const Stmt &stmt) {
        const auto _stmt = stmt.stmt;
        auto get_column_type = [_stmt](int col_index) -> std::string {
            int type = sqlite3_column_type(_stmt, col_index);
            switch (type) {
                    // clang-format off
            case SQLITE_INTEGER: return "I";
            case SQLITE_FLOAT: return "F";
            case SQLITE_TEXT: return "T";
            case SQLITE_BLOB: return "B";
            case SQLITE_NULL: return "N";
            default: return "U";
                    // clang-format on
            }
        };
        int col_count = sqlite3_column_count(_stmt);
        if (col_count == 0) {
            std::cout << "查询没有返回任何列。" << std::endl;
            return;
        }
        std::vector<std::string> headers;
        for (int i = 0; i < col_count; ++i) {
            const char *col_name = sqlite3_column_name(_stmt, i);
            std::string col_name_str = col_name ? col_name : "NULL";
            std::string col_type_str = get_column_type(i);
            headers.emplace_back(col_name_str + "(" + col_type_str + ")");
        }
        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << headers[i];
            if (i < headers.size() - 1) std::cout << "\t";
        }
        std::cout << std::endl;
        uint64_t count = 0;
        while (run(stmt)) {
            for (int i = 0; i < col_count; ++i) {
                const char *text = reinterpret_cast<const char *>(sqlite3_column_text(_stmt, i));
                std::cout << std::setw(headers[i].size()) << (text ? text : "NULL");
                if (i < col_count - 1) std::cout << "\t";
            }
            std::cout << std::endl;
            count++;
        }
        std::cout << "查询到 " << count << " 条记录。" << std::endl;
    }
};

using namespace sqlcpp;

Database db;

void create_tables() {
    {
        auto sql = Table("user")
                           .content(Col("id").INT().primary_key().not_null().auto_increment())
                           .content(Col("name").TEXT().not_null())
                           .content(Col("age").INT().not_null())
                           .build();
        db.exec(sql);
    }
    {
        auto sql = Table("project")
                           .content(Col("id").INT().primary_key().not_null().auto_increment())
                           .content(Col("name").TEXT().not_null())
                           .content(Col("owner").INT().not_null())
                           .content(ForeignKey("owner", "user", "id"))
                           .build();
        db.exec(sql);
    }
    {
        auto sql = Table("task")
                           .content(Col("project").INT().not_null())
                           .content(Col("user").INT().not_null())
                           .content(Col("name").TEXT().not_null().unique())
                           .content(Col("active").INT().not_null().default_val(true))
                           .content(PrimaryKey("project", "user"))
                           .build();
        db.exec(sql);
    }
}

void insert_data() {
    From p("project", "p");
    From u("user", "u");
    From t("task", "t");

    int64_t uid;
    {
        auto stmt = db.prepare(Insert(u).columns("name", "age").val("yuanlu", 25).build());
        db.run(stmt);
        uid = db.row_id();
        std::cout << "uid: " << uid << std::endl;
    }
    int64_t uid2;
    {
        auto stmt = db.prepare(Insert(u).columns("name", "age").val().build());
        db.bind(sqlite3_bind_text, stmt, 1, "task1", -1, SQLITE_STATIC);
        db.bind(sqlite3_bind_int, stmt, 2, 1);
        db.run(stmt);
        uid2 = db.row_id();
        db.reset(stmt);
        db.bind(sqlite3_bind_text, stmt, 1, "bar", -1, SQLITE_STATIC);
        db.bind(sqlite3_bind_int, stmt, 2, 999);
        db.run(stmt);
    }
    int64_t pid;
    {
        auto stmt = db.prepare(Insert(p).columns("name", "owner").val("proj1").build());
        db.bind(sqlite3_bind_int, stmt, 1, uid);
        db.run(stmt);
        pid = db.row_id();
    }

    {
        auto stmt = db.prepare(Insert(t).columns("project", "user", "name", "active").val().val().build());
        db.bind(sqlite3_bind_int, stmt, 1, pid);
        db.bind(sqlite3_bind_int, stmt, 2, uid);
        db.bind(sqlite3_bind_text, stmt, 3, "task1", -1, SQLITE_STATIC);
        db.bind(sqlite3_bind_int, stmt, 4, 1);
        db.bind(sqlite3_bind_int, stmt, 5, pid);
        db.bind(sqlite3_bind_int, stmt, 6, uid2);
        db.bind(sqlite3_bind_text, stmt, 7, "task2", -1, SQLITE_STATIC);
        db.bind(sqlite3_bind_int, stmt, 8, 1);
        db.run(stmt);
    }
}

void select_data() {
    From p("project", "p");
    From u("user", "u");
    From t("task", "t");

    {
        auto sql = Select(u["id"].as("uid"), u["name"])
                           .from(u)
                           .where(u["age"] > 20)
                           .build();
        auto stmt = db.prepare(sql);
        db.run_select(stmt);
    }
    {
        auto sql = Select("id", "name")
                           .from("user")
                           .where(Field("age") > 20)
                           .build();
        auto stmt = db.prepare(sql);
        db.run_select(stmt);
    }
    {
        auto sql = Select(
                           t["name"], t["project"].as("pid"), t["user"].as("uid"), t["active"],
                           u["name"].as("username"), u["age"].as("userage"), p["name"].as("pname"), p["owner"].as("powner"))
                           .from(t
                                         .join(LEFT_JOIN, p, t["project"] == p["id"])
                                         .join(LEFT_JOIN, u, t["user"] == u["id"]))
                           .where(t["active"] == true && t["name"] == p["name"] || u["age"] < VAR(1))
                           .order_by(t["name"].desc())
                           .build();
        auto stmt = db.prepare(sql);
        db.bind(sqlite3_bind_int, stmt, 1, 20);
        db.run_select(stmt);
    }
}

void update_data() {
    static const std::string NEW_VAL = "YUANLU123";
    {
        auto sql = Update("user", "name", VAR)
                           .where(Field("id") == 1)
                           .build();

        auto stmt = db.prepare(sql);
        db.bind(sqlite3_bind_text, stmt, 1, NEW_VAL.c_str(), -1, SQLITE_STATIC);
        db.run(stmt);
    }
    {
        auto sql = Select("name")
                           .from("user")
                           .where(Field("id") == 1)
                           .build();
        auto stmt = db.prepare(sql);
        db.run(stmt);
        const char *text = reinterpret_cast<const char *>(sqlite3_column_text(stmt.stmt, 0));
        if (text != NEW_VAL) {
            std::cerr << "UPDATE ERROR: " << text << " != " << NEW_VAL << std::endl;
            throw std::runtime_error("UPDATE ERROR");
        }
    }
}

void delete_data() {
    {
        auto sql = Delete("user", Field("age") > 100).build();
        auto stmt = db.prepare(sql);
        db.run(stmt);
    }
}

/// @brief test
int main() {
    std::cout << "SQLite version: " << sqlite3_libversion() << std::endl;
    create_tables();
    insert_data();
    select_data();
    update_data();
    delete_data();
    return 0;
}