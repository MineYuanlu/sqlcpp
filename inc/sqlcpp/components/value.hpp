/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_VALUE__HPP_GUARD
#define SQLCPP_COMPONENTS_VALUE__HPP_GUARD
#include "sqlcpp/components/expr.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/defs.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
namespace sqlcpp {

    /// @brief 原生值, 不会被转义
    /// @note 注意数据安全
    struct RawValue final : public Builder {
        std::string raw_value_;
        explicit RawValue(std::string raw_value);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    /// @brief 普通值, 会被转义
    /// @details 支持数字、字符串、布尔
    struct Value final : public Builder {
        std::variant<std::string, int64_t, uint64_t, double, float, bool> value_;

        /// @brief 从字符串构造Value
        /// @details 字符串会被转义
        Value(const char *value);
        /// @brief 从字符串构造Value
        /// @details 字符串会被转义
        Value(std::string value);
        Value(int64_t value);
        Value(uint64_t value);
        Value(double value);
        Value(float value);
        Value(bool value);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    /// @brief 二进制值, 会被转义
    struct BlobValue final : public Builder {
        /// @brief 从指针构造BlobValue
        /// @param data 二进制数据指针
        /// @param length 二进制数据长度
        /// @param copy 是否复制数据(对于copy为false的情况, 构造函数不会复制数据, 因此需要保证数据在生命周期内有效)
        BlobValue(const void *data, size_t length, bool copy = false);
        /// @brief 从数据向量构造BlobValue
        /// @details 允许任意类型的vector, 但会被转化为uint8_t*使用
        /// @param data 数据向量
        /// @param copy 是否复制数据(对于copy为false的情况, 构造函数不会复制数据, 因此需要保证数据在生命周期内有效)
        template<typename T>
        BlobValue(const std::vector<T> &data, bool copy = true)
            : BlobValue(data.data(), data.size() * sizeof(T), copy) {}
        /// @brief 从字符串构造BlobValue
        /// @param data 字符串数据
        /// @param copy 是否复制数据(对于copy为false的情况, 构造函数不会复制数据, 因此需要保证数据在生命周期内有效)
        explicit BlobValue(const std::string &data, bool copy = true)
            : BlobValue(data.data(), data.size(), copy) {}

        /// @brief 二进制数据指针
        [[nodiscard]] inline const uint8_t *data() const noexcept { return data_ptr_.get(); }
        /// @brief 二进制数据长度
        [[nodiscard]] inline size_t size() const noexcept { return data_len_; }

        /// @details MYSQL: 0x, SQLITE: x'
        /// @details 大写HEX表示
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

    private:
        std::shared_ptr<const uint8_t> data_ptr_;///< 二进制数据指针, 对于copy为false, 则没有deleter
        size_t data_len_ = 0;                    ///< 二进制数据长度
    };


    /// @brief 空值, 表示NULL
    struct NullValue final : public Builder {
        /// @see NULL_VALUE
        explicit NullValue() = default;
        /// @see NULL_VALUE
        NullValue(std::nullptr_t);
        /// @see NULL_VALUE
        NullValue(std::nullopt_t);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
    static const NullValue NULL_VALUE{};///< 空值, 表示NULL

    struct IndexedVarValue;
    /// @brief 普通占位符, 表示形式为"?", 可以使用VAR(N)代表有索引的占位符
    struct VarValue : public Builder, public VarBuilder {
        VarValue() = default;

        /// @brief 索引占位符, 索引值不会出现在sql中，而是需要使用语句的.get_var_map()获取索引映射对象
        /// @details index从0开始，使用时调用var_map.bind(binder, args...) 来映射参数
        //// @see VarMap
        IndexedVarValue operator()(size_t index) const;
        /// @brief 索引占位符, 索引值不会出现在sql中，而是需要使用语句的.get_var_map()获取索引映射对象
        /// @details index从0开始，使用时调用var_map.bind(binder, args...) 来映射参数
        //// @see VarMap
        IndexedVarValue operator[](size_t index) const;

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };

    /// @brief 数据占位符, 表示形式为"?", 可以使用VAR(N)代表有索引的占位符
    static const VarValue VAR{};

    /// @brief 带有索引的占位符, 表示形式为"?"
    //// @see VarMap
    struct IndexedVarValue final : public VarValue {
        size_t index_;///< 索引值, 从0开始
        explicit IndexedVarValue(size_t index);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };


    /// @brief 所有形似值表示的类
    /// @details 包括值(Value, Raw, Null, Blob), 占位符(Var, IndexedVar), 字段(Field, RawField, ExprLike)
    struct ValueLike final : public Builder, public VarBuilder {
        std::variant<Value, RawValue, NullValue, IndexedVarValue, VarValue, BlobValue, Field, RawField, ExprLike> value_;
        ValueLike(Value value);
        ValueLike(RawValue value);
        ValueLike(NullValue value);
        ValueLike(IndexedVarValue value);
        ValueLike(VarValue value);
        ValueLike(BlobValue value);
        ValueLike(Field value);
        ValueLike(RawField value);
        ValueLike(ExprLike value);
        ValueLike(const char *value);
        ValueLike(std::string value);
        ValueLike(int64_t value);
        ValueLike(uint64_t value);
        ValueLike(double value);
        ValueLike(float value);
        ValueLike(bool value);
        /// @brief 对于任意宽度的有符号整数, 构造ValueLike时会自动转换为int64_t
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type * = nullptr>
        ValueLike(T value) : value_(Value{static_cast<int64_t>(value)}) {}
        /// @brief 对于任意宽度的无符号整数, 构造ValueLike时会自动转换为uint64_t
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type * = nullptr>
        ValueLike(T value) : value_(Value{static_cast<uint64_t>(value)}) {}
        ValueLike(std::nullptr_t);
        ValueLike(std::nullopt_t);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_VALUE__HPP_GUARD
