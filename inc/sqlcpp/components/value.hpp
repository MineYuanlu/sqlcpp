/// Copyright (c) 2024 MineYuanlu
/// Author: MineYuanlu
/// Github: https://github.com/MineYuanlu/sqlcpp
/// Licence: MIT
#ifndef SQLCPP_COMPONENTS_VALUE__HPP_GUARD
#define SQLCPP_COMPONENTS_VALUE__HPP_GUARD
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/defs.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
namespace sqlcpp {

    struct RawValue final : public Builder {
        std::string raw_value_;
        explicit RawValue(std::string raw_value);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };


    struct Value final : public Builder {
        std::variant<std::string, int64_t, uint64_t, double, float, bool> value_;

        Value(const char *value);
        Value(std::string value);
        Value(int64_t value);
        Value(uint64_t value);
        Value(double value);
        Value(float value);
        Value(bool value);

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    struct BlobValue final : public Builder {
        BlobValue(const void *data, size_t length, bool copy = false);
        template<typename T>
        BlobValue(const std::vector<T> &data, bool copy = true)
            : BlobValue(data.data(), data.size() * sizeof(T), copy) {}
        explicit BlobValue(const std::string &data, bool copy = true)
            : BlobValue(data.data(), data.size(), copy) {}


        [[nodiscard]] inline const uint8_t *data() const noexcept { return data_ptr_.get(); }
        [[nodiscard]] inline size_t size() const noexcept { return data_len_; }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;

    private:
        std::shared_ptr<const uint8_t> data_ptr_;
        size_t data_len_ = 0;
    };


    struct NullValue final : public Builder {
        explicit NullValue() = default;
        NullValue(std::nullptr_t);
        NullValue(std::nullopt_t);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
    static const NullValue NULL_VALUE{};

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


    struct ValueLike final : public Builder, public VarBuilder {
        std::variant<Value, RawValue, NullValue, IndexedVarValue, VarValue, BlobValue, Field, RawField, FuncField> value_;
        ValueLike(Value value);
        ValueLike(RawValue value);
        ValueLike(NullValue value);
        ValueLike(IndexedVarValue value);
        ValueLike(VarValue value);
        ValueLike(BlobValue value);
        ValueLike(Field value);
        ValueLike(RawField value);
        ValueLike(FuncField value);
        ValueLike(const char *value);
        ValueLike(std::string value);
        ValueLike(int64_t value);
        ValueLike(uint64_t value);
        ValueLike(double value);
        ValueLike(float value);
        ValueLike(bool value);
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type * = nullptr>
        ValueLike(T value) : value_(static_cast<int64_t>(value)) {}
        template<typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type * = nullptr>
        ValueLike(T value) : value_(static_cast<uint64_t>(value)) {}
        ValueLike(std::nullptr_t);
        ValueLike(std::nullopt_t);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
        void edit_var_map(VarMap &var_map) const override;
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_VALUE__HPP_GUARD
