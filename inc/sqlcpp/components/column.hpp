/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_COLUMN__HPP_GUARD
#define SQLCPP_COMPONENTS_COLUMN__HPP_GUARD

#include "sqlcpp/defs.hpp"
#include <optional>
#include <vector>
#include <tuple>
namespace sqlcpp {
    struct Column : public Builder {
        std::string name_;
        std::tuple<std::string, std::optional<int>, std::optional<int>> type_;

        bool NOT_NULL_ = false;
        std::optional<std::string> DEFAULT_ = {};
        bool UNIQUE_ = false;
        bool PRIMARY_KEY_ = false;
        bool AUTO_INCREMENT_ = false;
        bool FULL_TEXT_SEARCH_ = false;
        std::optional<std::string> COMMENT_ = {};
        std::optional<std::string> REFERENCES_ = {};
        std::optional<std::string> CHECK_ = {};
        std::optional<std::string> ON_UPDATE_ = {};
        std::optional<std::string> CHARACTER_SET_ = {};
        std::optional<std::string> COLLATE_ = {};

        Column(std::string f, std::string t = {});

        void build_s(std::ostream &oss, const Type &t) const override;

        Column &type(std::string type);
        Column &type(const std::string &type, int m);
        Column &type(const std::string &type, int m, int d);

        Column &not_null(bool v = true);

        Column &default_val(std::optional<std::string> v);
        /// 设置原始的DEFAULT值
        Column &default_raw(std::string v);
        template<class T>
        std::enable_if_t<std::is_arithmetic_v<T>, Column &> default_val(T value) {
            DEFAULT_ = std::to_string(value);
            return *this;
        }
        Column &default_val(std::nullptr_t);
        Column &default_CURRENT_TIMESTAMP();
        Column &default_UUID();

        Column &on_update_CURRENT_TIMESTAMP();

        Column &unique(bool v = true);

        Column &primary_key(bool v = true);

        Column &auto_increment(bool v = true);

        Column &comment(std::optional<std::string> v);

        Column &references(std::optional<std::string> v);
        Column &references(const std::string &table, const std::string &column_name);

        Column &check(std::optional<std::string> v);

        Column &character_set(std::optional<std::string> v);

        Column &collate(std::optional<std::string> v);

        /// @brief 全文搜索支持
        /// @details 由Table实现功能, 在Mysql下为"FULLTEXT INDEX", 在Sqlite下为"VIRTUAL TABLE"
        Column &fullTextSearch(bool v = true);


        // 数值类型
        Column &TINYINT(int m = 4);
        Column &BOOL();
        Column &BOOLEAN();
        Column &SMALLINT(int m = 6);
        Column &MEDIUMINT(int m = 9);
        Column &INT(int m = 11);
        Column &INTEGER(int m = 11);
        Column &BIGINT(int m = 20);

        Column &FLOAT(int m = 10, int d = 2);
        Column &DOUBLE(int m = 16, int d = 4);
        Column &DOUBLE_PRECISION(int m = 16, int d = 4);
        Column &REAL(int m = 16, int d = 4);

        Column &DECIMAL(int m = 10, int d = 0);
        Column &DEC(int m = 10, int d = 0);
        Column &NUMERIC(int m = 10, int d = 0);

        // 日期和时间类型
        Column &DATE();
        Column &DATETIME(int fsp = 0);
        Column &TIMESTAMP(int fsp = 0);
        Column &TIME(int fsp = 0);
        Column &YEAR(int length = 4);

        // 字符串类型
        Column &CHAR(int m = 1);
        Column &VARCHAR(int m = 255);

        // 文本类型
        Column &TINYTEXT();
        Column &TEXT(int m = 65535);
        Column &MEDIUMTEXT();
        Column &LONGTEXT();

        // 二进制类型
        Column &BINARY(int m = 1);
        Column &VARBINARY(int m = 255);

        // BLOB 类型
        Column &TINYBLOB();
        Column &BLOB(int m = 65535);
        Column &MEDIUMBLOB();
        Column &LONGBLOB();

        // 其他
        Column &ENUM(std::string values);
        Column &SET(std::string values);
        Column &ENUM(std::initializer_list<std::string> values);
        Column &SET(std::initializer_list<std::string> values);

        // 空间数据类型
        Column &GEOMETRY();
        Column &POINT();
        Column &LINESTRING();
        Column &POLYGON();
        Column &MULTIPOINT();
        Column &MULTILINESTRING();
        Column &MULTIPOLYGON();
        Column &GEOMETRYCOLLECTION();

        // JSON 数据类型
        Column &JSON();
    };

    using Col = Column;


    struct PrimaryKey : public Builder {
        std::vector<std::string> columns_{};
        PrimaryKey(const std::string &f);
        PrimaryKey(const Column &f);
        PrimaryKey(std::vector<std::string> f);
        PrimaryKey(std::vector<Column> f);
        PrimaryKey(std::initializer_list<std::string> f);
        PrimaryKey(std::initializer_list<Column> f);
        template<
                typename... Args,
                typename = std::enable_if_t<
                        ((std::is_convertible_v<Args, std::string> || std::is_convertible_v<Args, Column>) &&...)>>
        PrimaryKey(Args &&...args) {
            columns_.reserve(sizeof...(args));
            (columns_.emplace_back(convert(std::forward<Args>(args))), ...);
        }

        void build_s(std::ostream &oss, const Type &t) const override;

    private:
        inline static std::string convert(const std::string &s) { return s; }
        inline static std::string convert(const Column &c) { return c.name_; }
    };


    struct Unique : public Builder {
        std::vector<std::string> columns_{};
        Unique(const std::string &f);
        Unique(const Column &f);
        Unique(std::vector<std::string> f);
        Unique(std::vector<Column> f);
        Unique(std::initializer_list<std::string> f);
        Unique(std::initializer_list<Column> f);
        template<
                typename... Args,
                typename = std::enable_if_t<
                        ((std::is_convertible_v<Args, std::string> || std::is_convertible_v<Args, Column>) &&...)>>
        Unique(Args &&...args) {
            columns_.reserve(sizeof...(args));
            (columns_.emplace_back(convert(std::forward<Args>(args))), ...);
        }

        void build_s(std::ostream &oss, const Type &t) const override;

    private:
        inline static std::string convert(const std::string &s) { return s; }
        inline static std::string convert(const Column &c) { return c.name_; }
    };


    struct ForeignKey : public Builder {
        enum Action {
            ///如果在父表中删除了一行，引用该行的所有子表记录也会被自动删除。
            ///如果在父表中更新了一行，引用该行的所有子表记录中的对应值也会被自动更新。
            CASCADE,
            ///如果在父表中删除了一行，引用该行的所有子表记录中的外键值会被设置为 NULL。子表的外键列必须允许 NULL 值。
            ///如果在父表中更新了一行，引用该行的所有子表记录中的外键值会被设置为 NULL。
            SET_NULL,
            ///如果在父表中删除了一行，引用该行的所有子表记录中的外键值会被设置为默认值。注意，MySQL 不直接支持 SET DEFAULT，这种行为通常在其他数据库管理系统（如 PostgreSQL）中使用。
            ///如果在父表中更新了一行，引用该行的所有子表记录中的外键值会被设置为默认值。MySQL 也不直接支持此选项。
            SET_DEFAULT,
            ///如果在父表中有被引用的记录，则禁止删除该记录。这是默认行为之一。
            ///如果在父表中有被引用的记录，则禁止更新该记录。这是默认行为之一。
            RESTRICT,
            ///与 RESTRICT 类似，如果在父表中有被引用的记录，则禁止删除该记录。实际上，NO ACTION 和 RESTRICT 在 MySQL 中的行为是相同的。
            ///与 RESTRICT 类似，如果在父表中有被引用的记录，则禁止更新该记录。
            NO_ACTION,
        };
        std::string column_{};
        std::string fk_table_{};
        std::string fk_column_{};
        std::optional<Action> ON_DELETE_{};
        std::optional<Action> ON_UPDATE_{};

        ForeignKey(std::string f);
        ForeignKey(Column f);
        ForeignKey(std::string f, std::string fk_table, std::string fk_column);
        ForeignKey(Column f, std::string fk_table, std::string fk_column);
        ForeignKey(std::string f, std::string fk_table, Column fk_column);
        ForeignKey(Column f, std::string fk_table, Column fk_column);

        ForeignKey &fk(std::string table, std::string column);
        ForeignKey &fk(std::string table, Column column);
        ForeignKey &on_delete(const std::optional<Action> &a);
        ForeignKey &on_update(const std::optional<Action> &a);
        void build_s(std::ostream &oss, const Type &t) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_COLUMN__HPP_GUARD
