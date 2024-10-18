#include "sqlcpp/components/value.hpp"
#include "sqlcpp/components/field.hpp"
#include "sqlcpp/str.hpp"
#include "sqlcpp/types.hpp"
#include <cstring>
namespace sqlcpp {
    using namespace sqlcpp::str;

    RawValue::RawValue(std::string raw_value) : raw_value_(std::move(raw_value)) {}
    void RawValue::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << raw_value_;
    }


    Value::Value(const char *value) : value_(std::string{value}) {}
    Value::Value(std::string value) : value_(std::move(value)) {}
    Value::Value(int64_t value) : value_(value) {}
    Value::Value(uint64_t value) : value_(value) {}
    Value::Value(double value) : value_(value) {}
    Value::Value(float value) : value_(value) {}
    Value::Value(bool value) : value_(value) {}
    void Value::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        if (std::holds_alternative<std::string>(value_))
            oss << safe_value(std::get<std::string>(value_));
        else if (std::holds_alternative<bool>(value_))
            oss << (std::get<bool>(value_) ? '1' : '0');
        else
            std::visit([&](const auto &v) { oss << v; }, value_);
    }


    BlobValue::BlobValue(const void *data, size_t length, bool copy) : data_len_(length) {
        if (copy) {
            auto *new_data = new uint8_t[length];
            std::memcpy(new_data, data, length);
            data_ptr_ = std::shared_ptr<const uint8_t>(new_data, std::default_delete<const uint8_t[]>());
        } else {
            data_ptr_ = std::shared_ptr<const uint8_t>(
                    reinterpret_cast<const uint8_t *>(data),
                    [](const uint8_t *) {});
        }
    }
    void BlobValue::build_s(std::ostream &oss, const Type &t) const {
        if (t == MYSQL)
            oss << "0x";
        else if (t == SQLITE)
            oss << "x'";

        hex_stream(oss, data_ptr_.get(), data_len_);

        if (t == SQLITE)
            oss << '\'';
    }


    NullValue::NullValue(std::nullptr_t) {}
    NullValue::NullValue(std::nullopt_t) {}
    void NullValue::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << "NULL";
    }


    IndexedVarValue::IndexedVarValue(size_t index) : index_(index) {}
    void IndexedVarValue::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << '?';
    }


    void VarValue::build_s(std::ostream &oss, [[maybe_unused]] const Type &t) const {
        oss << '?';
    }


    ValueLike::ValueLike(Value value) : value_(std::move(value)) {}
    ValueLike::ValueLike(RawValue value) : value_(std::move(value)) {}
    ValueLike::ValueLike(NullValue value) : value_(std::move(value)) {}
    ValueLike::ValueLike(IndexedVarValue value) : value_(std::move(value)) {}
    ValueLike::ValueLike(VarValue value) : value_(std::move(value)) {}
    ValueLike::ValueLike(BlobValue value) : value_(std::move(value)) {}
    ValueLike::ValueLike(Field value) : value_(std::move(value)) {}
    ValueLike::ValueLike(RawField value) : value_(std::move(value)) {}
    ValueLike::ValueLike(FuncField value) : value_(std::move(value)) {}
    ValueLike::ValueLike(const char *value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(std::string value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(int64_t value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(uint64_t value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(double value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(float value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(bool value) : value_(Value(std::move(value))) {}
    ValueLike::ValueLike(std::nullptr_t) : value_(NULL_VALUE) {}
    ValueLike::ValueLike(std::nullopt_t) : value_(NULL_VALUE) {}
    void ValueLike::build_s(std::ostream &oss, const Type &t) const {
        std::visit([&](const auto &arg) { arg.build_s(oss, t); }, value_);
    }
}// namespace sqlcpp
