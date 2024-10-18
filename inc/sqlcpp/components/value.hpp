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

    struct IndexedVarValue final : public Builder {
        size_t index_;
        explicit IndexedVarValue(size_t index);
        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };

    struct VarValue final : public Builder {
        VarValue() = default;

        inline IndexedVarValue operator()(size_t index) const { return IndexedVarValue{index}; }
        inline IndexedVarValue operator[](size_t index) const { return IndexedVarValue{index}; }

        void build_s(std::ostream &oss, const Type &t = SQLCPP_DEFAULT_TYPE) const override;
    };
    static const VarValue VAR{};


    struct ValueLike final : public Builder {
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
    };
}// namespace sqlcpp
#endif// SQLCPP_COMPONENTS_VALUE__HPP_GUARD
