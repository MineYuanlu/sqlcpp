#include "sqlcpp/components/column.hpp"
#include "sqlcpp/str.hpp"
#include <cstddef>
#include <stdexcept>

namespace {
    static const std::unordered_map<std::string, std::string> mysqlToSqliteTypeMap = {
            {"TINYINT", "INTEGER"},
            {"BOOL", "INTEGER"},
            {"BOOLEAN", "INTEGER"},
            {"SMALLINT", "INTEGER"},
            {"MEDIUMINT", "INTEGER"},
            {"INT", "INTEGER"},
            {"INTEGER", "INTEGER"},
            {"BIGINT", "INTEGER"},
            {"FLOAT", "REAL"},
            {"DOUBLE", "REAL"},
            {"DOUBLE PRECISION", "REAL"},
            {"REAL", "REAL"},
            {"DECIMAL", "NUMERIC"},
            {"DEC", "NUMERIC"},
            {"NUMERIC", "NUMERIC"},
            {"DATE", "DATE"},
            {"DATETIME", "DATETIME"},
            {"TIMESTAMP", "DATETIME"},
            {"TIME", "TIME"},
            {"YEAR", "INTEGER"},
            {"CHAR", "TEXT"},
            {"VARCHAR", "TEXT"},
            {"TINYTEXT", "TEXT"},
            {"TEXT", "TEXT"},
            {"MEDIUMTEXT", "TEXT"},
            {"LONGTEXT", "TEXT"},
            {"BINARY", "BLOB"},
            {"VARBINARY", "BLOB"},
            {"TINYBLOB", "BLOB"},
            {"BLOB", "BLOB"},
            {"MEDIUMBLOB", "BLOB"},
            {"LONGBLOB", "BLOB"},
            {"ENUM", "TEXT"},
            {"SET", "TEXT"},
            {"GEOMETRY", "BLOB"},
            {"POINT", "BLOB"},
            {"LINESTRING", "BLOB"},
            {"POLYGON", "BLOB"},
            {"MULTIPOINT", "BLOB"},
            {"MULTILINESTRING", "BLOB"},
            {"MULTIPOLYGON", "BLOB"},
            {"GEOMETRYCOLLECTION", "BLOB"},
            {"JSON", "TEXT"}};
}// namespace

namespace sqlcpp {

    using namespace sqlcpp::str;

    Column::Column(std::string name, std::string type)
        : name_(std::move(name)), type_(std::move(type), std::nullopt, std::nullopt) {
    }
    void Column::build_s(std::ostream &oss, const Type &t) const {
        if (name_.empty()) throw std::invalid_argument("[sqlcpp] Column name is empty");
        if (std::get<0>(type_).empty()) throw std::invalid_argument("[sqlcpp] Column type is empty");
        oss << safe_field(name_, t) << ' ';
        auto type = str::toUpperCase(std::get<0>(type_));
        if (t == MYSQL) {
            oss << type;
            if (std::get<1>(type_)) {
                oss << '(' << *std::get<1>(type_);
                if (std::get<2>(type_)) {
                    oss << ',' << *std::get<2>(type_);
                }
                oss << ')';
            }
        } else if (t == SQLITE) {
            auto itr = mysqlToSqliteTypeMap.find(type);
            oss << (itr == mysqlToSqliteTypeMap.end() ? type : itr->second);
        } else {
            throw std::invalid_argument("[sqlcpp] Unknown SQL type: " + std::string{t});
        }
        oss << ' ';

        if (CHARACTER_SET_)
            oss << "CHARACTER SET " << *CHARACTER_SET_ << ' ';
        if (COLLATE_)
            oss << "COLLATE " << *COLLATE_ << ' ';

        if (NOT_NULL_)
            oss << "NOT NULL ";

        if (PRIMARY_KEY_)
            oss << "PRIMARY KEY ";
        else if (UNIQUE_)
            oss << "UNIQUE ";

        if (AUTO_INCREMENT_) {
            switch (t) {
                case Types::MYSQL:
                    oss << "AUTO_INCREMENT ";
                    break;
                case Types::SQLITE:
                    oss << "AUTOINCREMENT ";
                    break;
            }
        }
        if (DEFAULT_)
            oss << "DEFAULT " << *DEFAULT_ << ' ';
        if (ON_UPDATE_)
            oss << "ON UPDATE " << *ON_UPDATE_ << ' ';
        if (COMMENT_ && t == MYSQL)
            oss << "COMMENT " << *COMMENT_ << ' ';
        if (CHECK_)
            oss << "CHECK (" << *CHECK_ << ')' << ' ';
        if (REFERENCES_)
            oss << "REFERENCES " << *REFERENCES_ << ' ';
    }
    Column &Column::type(std::string type) {
        type_ = std::make_tuple(std::move(type), std::nullopt, std::nullopt);
        return *this;
    }
    Column &Column::type(const std::string &type, int m) {
        type_ = std::make_tuple(std::move(type), m, std::nullopt);
        return *this;
    }
    Column &Column::type(const std::string &type, int m, int d) {
        type_ = std::make_tuple(std::move(type), std::nullopt, d);
        return *this;
    }
    Column &Column::not_null(bool v) {
        NOT_NULL_ = v;
        return *this;
    }
    Column &Column::default_val(std::optional<std::string> v) {
        DEFAULT_ = v ? safe_value(*v) : std::move(v);
        return *this;
    }
    Column &Column::default_raw(std::string v) {
        DEFAULT_ = std::move(v);
        return *this;
    }
    Column &Column::default_val(std::nullptr_t) {
        return default_raw("NULL");
    }
    Column &Column::default_CURRENT_TIMESTAMP() {
        return default_raw("CURRENT_TIMESTAMP");
    }
    Column &Column::default_UUID() {
        return default_raw("UUID()");
    }
    Column &Column::on_update_CURRENT_TIMESTAMP() {
        return default_raw("CURRENT_TIMESTAMP");
    }
    Column &Column::unique(bool v) {
        UNIQUE_ = v;
        return *this;
    }
    Column &Column::primary_key(bool v) {
        PRIMARY_KEY_ = v;
        if (v) NOT_NULL_ = true;
        return *this;
    }
    Column &Column::auto_increment(bool v) {
        AUTO_INCREMENT_ = v;
        return *this;
    }
    Column &Column::comment(std::optional<std::string> v) {
        COMMENT_ = v ? safe_value(*v) : std::move(v);
        return *this;
    }
    Column &Column::references(std::optional<std::string> v) {
        REFERENCES_ = v;
        return *this;
    }
    Column &Column::references(const std::string &table, const std::string &column_name) {
        REFERENCES_ = table + '(' + column_name + ')';
        return *this;
    }
    Column &Column::check(std::optional<std::string> v) {
        CHECK_ = v;
        return *this;
    }
    Column &Column::character_set(std::optional<std::string> v) {
        CHARACTER_SET_ = v;
        return *this;
    }
    Column &Column::collate(std::optional<std::string> v) {
        COLLATE_ = v;
        return *this;
    }
    Column &Column::fullTextSearch(bool v) {
        FULL_TEXT_SEARCH_ = v;
        return *this;
    }

    // 数值类型
    Column &Column::TINYINT(int m) { return type("tinyint", m); }
    Column &Column::BOOL() { return TINYINT(1); }
    Column &Column::BOOLEAN() { return BOOL(); }
    Column &Column::SMALLINT(int m) { return type("smallint", m); }
    Column &Column::MEDIUMINT(int m) { return type("mediumint", m); }
    Column &Column::INT(int m) { return type("int", m); }
    Column &Column::INTEGER(int m) { return INT(m); }
    Column &Column::BIGINT(int m) { return type("bigint", m); }

    Column &Column::FLOAT(int m, int d) { return type("float", m, d); }
    Column &Column::DOUBLE(int m, int d) { return type("double", m, d); }
    Column &Column::DOUBLE_PRECISION(int m, int d) { return DOUBLE(m, d); }
    Column &Column::REAL(int m, int d) { return DOUBLE(m, d); }

    Column &Column::DECIMAL(int m, int d) { return type("decimal", m, d); }
    Column &Column::DEC(int m, int d) { return DECIMAL(m, d); }
    Column &Column::NUMERIC(int m, int d) { return DECIMAL(m, d); }

    // 日期和时间类型
    Column &Column::DATE() { return type("date"); }
    Column &Column::DATETIME(int fsp) { return type("datetime", fsp); }
    Column &Column::TIMESTAMP(int fsp) { return type("timestamp", fsp); }
    Column &Column::TIME(int fsp) { return type("time", fsp); }
    Column &Column::YEAR(int length) { return type("year", length); }

    // 字符串类型
    Column &Column::CHAR(int m) { return type("char", m); }
    Column &Column::VARCHAR(int m) { return type("varchar", m); }

    // 文本类型
    Column &Column::TINYTEXT() { return type("tinytext"); }
    Column &Column::TEXT(int m) { return type("text", m); }
    Column &Column::MEDIUMTEXT() { return type("mediumtext"); }
    Column &Column::LONGTEXT() { return type("longtext"); }

    // 二进制类型
    Column &Column::BINARY(int m) { return type("binary", m); }
    Column &Column::VARBINARY(int m) { return type("varbinary", m); }

    // BLOB 类型
    Column &Column::TINYBLOB() { return type("tinyblob"); }
    Column &Column::BLOB(int m) { return type("blob", m); }
    Column &Column::MEDIUMBLOB() { return type("mediumblob"); }
    Column &Column::LONGBLOB() { return type("longblob"); }

    // 其他
    Column &Column::ENUM(std::string values) { return type("enum(" + values + ")"); }
    Column &Column::SET(std::string values) { return type("set(" + values + ")"); }
    Column &Column::ENUM(std::initializer_list<std::string> values) { return ENUM(safe_values(values)); }
    Column &Column::SET(std::initializer_list<std::string> values) { return SET(safe_values(values)); }

    // 空间数据类型
    Column &Column::GEOMETRY() { return type("geometry"); }
    Column &Column::POINT() { return type("point"); }
    Column &Column::LINESTRING() { return type("linestring"); }
    Column &Column::POLYGON() { return type("polygon"); }
    Column &Column::MULTIPOINT() { return type("multipoint"); }
    Column &Column::MULTILINESTRING() { return type("multilinestring"); }
    Column &Column::MULTIPOLYGON() { return type("multipolygon"); }
    Column &Column::GEOMETRYCOLLECTION() { return type("geometrycollection"); }

    // JSON 数据类型
    Column &Column::JSON() { return type("json"); }


    PrimaryKey::PrimaryKey(const std::string &column) : columns_({column}) {}
    PrimaryKey::PrimaryKey(const Column &column) : columns_({column.name_}) {}
    PrimaryKey::PrimaryKey(std::vector<std::string> columns) : columns_(std::move(columns)) {}
    PrimaryKey::PrimaryKey(std::vector<Column> columns) {
        columns_.reserve(columns.size());
        for (const auto &column: columns) columns_.emplace_back(column.name_);
    }
    PrimaryKey::PrimaryKey(std::initializer_list<std::string> columns) : columns_(std::move(columns)) {}
    PrimaryKey::PrimaryKey(std::initializer_list<Column> columns) {
        columns_.reserve(columns.size());
        for (const auto &column: columns) columns_.emplace_back(column.name_);
    }


    void PrimaryKey::build_s(std::ostream &oss, const Type &t) const {
        oss << "PRIMARY KEY (";
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << safe_field(columns_[i], t);
        }
        oss << ')';
    }
    Unique::Unique(const std::string &column) : columns_({column}) {}
    Unique::Unique(const Column &column) : columns_({column.name_}) {}
    Unique::Unique(std::vector<std::string> columns) : columns_(std::move(columns)) {}
    Unique::Unique(std::vector<Column> columns) {
        columns_.reserve(columns.size());
        for (const auto &column: columns) columns_.emplace_back(column.name_);
    }
    Unique::Unique(std::initializer_list<std::string> columns) : columns_(std::move(columns)) {}
    Unique::Unique(std::initializer_list<Column> columns) {
        columns_.reserve(columns.size());
        for (const auto &column: columns) columns_.emplace_back(column.name_);
    }
    void Unique::build_s(std::ostream &oss, const Type &t) const {
        oss << "UNIQUE (";
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << safe_field(columns_[i], t);
        }
        oss << ')';
    }
    ForeignKey::ForeignKey(std::string column) : column_(std::move(column)) {}
    ForeignKey::ForeignKey(Column column) : column_(column.name_) {}
    ForeignKey::ForeignKey(std::string column, std::string fk_table, std::string fk_column) : column_(std::move(column)), fk_table_(std::move(fk_table)), fk_column_(std::move(fk_column)) {}
    ForeignKey::ForeignKey(Column column, std::string fk_table, std::string fk_column) : column_(column.name_), fk_table_(std::move(fk_table)), fk_column_(std::move(fk_column)) {}
    ForeignKey::ForeignKey(std::string column, std::string fk_table, Column fk_column) : column_(std::move(column)), fk_table_(std::move(fk_table)), fk_column_(fk_column.name_) {}
    ForeignKey::ForeignKey(Column column, std::string fk_table, Column fk_column) : column_(column.name_), fk_table_(std::move(fk_table)), fk_column_(fk_column.name_) {}
    ForeignKey &ForeignKey::fk(std::string table, std::string column) {
        fk_table_ = std::move(table);
        fk_column_ = std::move(column);
        return *this;
    }
    ForeignKey &ForeignKey::fk(std::string table, Column column) {
        fk_table_ = std::move(table);
        fk_column_ = column.name_;
        return *this;
    }
    ForeignKey &ForeignKey::on_delete(const std::optional<Action> &a) {
        ON_DELETE_ = a;
        return *this;
    }
    ForeignKey &ForeignKey::on_update(const std::optional<Action> &a) {
        ON_UPDATE_ = a;
        return *this;
    }
    std::string action(const ForeignKey::Action &a) {
        // clang-format off
        switch (a) {
            case ForeignKey::CASCADE:     return "CASCADE";
            case ForeignKey::SET_NULL:    return "SET NULL";
            case ForeignKey::SET_DEFAULT: return "SET DEFAULT";
            case ForeignKey::RESTRICT:    return "RESTRICT";
            case ForeignKey::NO_ACTION:   return "NO ACTION";
        }
        // clang-format on
        throw std::invalid_argument("[sqlcpp] Unknown Action: " + std::to_string(a));
    }
    void ForeignKey::build_s(std::ostream &oss, const Type &t) const {
        oss << "FOREIGN KEY (" << safe_field(column_, t) << ") REFERENCES " << safe_table(fk_table_, t) << '(' << safe_field(fk_column_, t) << ')';
        if (ON_DELETE_) oss << " ON DELETE " << action(*ON_DELETE_);
        if (ON_UPDATE_) oss << " ON UPDATE " << action(*ON_UPDATE_);
    }
}// namespace sqlcpp