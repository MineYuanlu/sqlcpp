/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_COLUMN__HPP_GUARD
#define SQLCPP_COMPONENTS_COLUMN__HPP_GUARD

#include "sqlcpp/components/field.hpp"
#include "sqlcpp/defs.hpp"
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
namespace sqlcpp {

    struct Column;
    /// @brief 外键定义, 用于构建CREATE TABLE语句
    struct ForeignKey final : public Builder {
        /// @brief 外键行为
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
        std::vector<std::string> column_{};   ///< 外键列名
        std::string fk_table_{};              ///< 引用表名
        std::vector<std::string> fk_column_{};///< 引用列名
        std::optional<Action> ON_DELETE_{};   ///< 删除行为
        std::optional<Action> ON_UPDATE_{};   ///< 更新行为

        ForeignKey() = default;
        ForeignKey(std::string f);
        ForeignKey(Column f);
        ForeignKey(std::string f, std::string fk_table, std::string fk_column);
        ForeignKey(Column f, std::string fk_table, std::string fk_column);
        ForeignKey(std::string f, std::string fk_table, Column fk_column);
        ForeignKey(Column f, std::string fk_table, Column fk_column);

        /// @brief 设置外键列名
        template<typename... Args>
        ForeignKey &col(Args... f);
        /// @brief 设置外键引用表名及列名
        template<typename... Args>
        ForeignKey &fk(std::string table, Args... column);
        /// @brief 设置外键删除行为
        ForeignKey &on_delete(const std::optional<Action> &a);
        /// @brief 设置外键更新行为
        ForeignKey &on_update(const std::optional<Action> &a);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

    private:
        static std::string getColName(std::string col);
        static std::string getColName(const Column &col);
    };

    template<typename... Args>
    ForeignKey &ForeignKey::col(Args... f) {
        column_.clear();
        column_.reserve(sizeof...(f));
        (column_.emplace_back(getColName(std::forward<Args>(f))), ...);
        return *this;
    }
    template<typename... Args>
    ForeignKey &ForeignKey::fk(std::string table, Args... column) {
        fk_table_ = std::move(table);
        fk_column_.clear();
        fk_column_.reserve(sizeof...(column));
        (fk_column_.emplace_back(getColName(std::forward<Args>(column))), ...);
        return *this;
    }


    /// @brief 字段定义, 用于构建CREATE TABLE语句
    struct Column final : public Builder {
        using RefData = std::tuple<std::string, std::string, std::optional<ForeignKey::Action>, std::optional<ForeignKey::Action>>;
        using TypeData = std::tuple<std::string, std::optional<int>, std::optional<int>>;

        std::string name_{};///< 字段名
        TypeData type_{};   ///< 字段类型, 长度, 小数点

        bool NOT_NULL_ = false;                     ///< 是否为NOT NULL字段
        std::optional<std::string> DEFAULT_{};      ///< 默认值
        bool UNIQUE_ = false;                       ///< 是否为UNIQUE字段
        bool PRIMARY_KEY_ = false;                  ///< 是否为PRIMARY KEY字段
        bool AUTO_INCREMENT_ = false;               ///< 是否为AUTO_INCREMENT字段
        bool FULL_TEXT_SEARCH_ = false;             ///< 是否为FULL_TEXT_SEARCH字段
        std::optional<std::string> COMMENT_{};      ///< 字段注释
        std::optional<RefData> REFERENCES_{};       ///< 外键引用
        std::optional<std::string> CHECK_{};        ///< 字段检查
        std::optional<std::string> ON_UPDATE_{};    ///< ON UPDATE
        std::optional<std::string> CHARACTER_SET_{};///< 字符集
        std::optional<std::string> COLLATE_{};      ///< 排序规则

        /// @brief 构造字段
        /// @param f 字段名
        /// @param t 字段类型, 默认为空
        Column(std::string f, std::string t = {});

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

        /// @brief 指定类型名称
        Column &type(std::string type);
        /// @brief 指定类型及长度
        Column &type(const std::string &type, int m);
        /// @brief 指定类型、长度及小数点位数
        Column &type(const std::string &type, int m, int d);

        /// @brief 指定为NOT NULL字段
        Column &not_null(bool v = true);

        /// @brief 设置默认值
        Column &default_val(std::optional<std::string> v);
        /// 设置原始的DEFAULT值
        Column &default_raw(std::string v);
        /// @brief 设置默认值
        template<class T>
        std::enable_if_t<std::is_arithmetic_v<T>, Column &> default_val(T value) {
            DEFAULT_ = std::to_string(value);
            return *this;
        }
        /// @brief 设置默认值
        Column &default_val(std::nullptr_t);
        /// @brief 设置默认值为 CURRENT_TIMESTAMP
        Column &default_CURRENT_TIMESTAMP();
        /// @brief 设置默认值为 UUID()
        Column &default_UUID();

        /// @brief 设置ON UPDATE CURRENT_TIMESTAMP
        Column &on_update_CURRENT_TIMESTAMP();

        /// @brief 指定为UNIQUE字段
        Column &unique(bool v = true);

        /// @brief 指定为PRIMARY KEY字段
        Column &primary_key(bool v = true);

        /// @brief 指定为AUTO_INCREMENT字段
        Column &auto_increment(bool v = true);

        /// @brief 指定字段注释
        Column &comment(std::optional<std::string> v);

        /// @brief 指定外键引用
        /// @param table 外键表名
        /// @param column_name 外键列名
        /// @param on_delete 删除行为
        /// @param on_update 更新行为
        Column &references(const std::string &table, const std::string &column_name,
                           std::optional<ForeignKey::Action> on_delete = std::nullopt,
                           std::optional<ForeignKey::Action> on_update = std::nullopt);

        /// @brief 指定字段检查
        Column &check(std::optional<std::string> v);

        /// @brief 指定字符集
        Column &character_set(std::optional<std::string> v);

        /// @brief 指定排序规则
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


    /// @brief 主键定义, 用于构建CREATE TABLE语句
    struct PrimaryKey final : public Builder {
        std::vector<std::string> columns_{};///< 主键列名
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

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

    private:
        inline static std::string convert(const std::string &s) { return s; }
        inline static std::string convert(const Column &c) { return c.name_; }
    };


    /// @brief 唯一索引定义, 用于构建CREATE TABLE语句
    struct Unique final : public Builder {
        std::vector<std::string> columns_{};///< 唯一索引列名
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

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

    private:
        inline static std::string convert(const std::string &s) { return s; }
        inline static std::string convert(const Column &c) { return c.name_; }
    };


}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_COLUMN__HPP_GUARD
