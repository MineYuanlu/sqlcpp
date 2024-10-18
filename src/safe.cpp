#include "sqlcpp/str.hpp"
#include <sstream>
namespace sqlcpp::str {
    std::string package_string(const std::string &input, char package) {
        std::string output;
        output.reserve(input.size() * 2 + 2);

        output.push_back(package);

        for (char c: input) {
            if (c == package) {
                output.push_back('\\');
            }
            output.push_back(c);
        }

        output.push_back(package);

        return output;
    }
    std::string join(const std::initializer_list<std::string> &list, const std::string &joiner,
                     const std::function<std::string(std::string)> &val_trans,
                     const std::string &prefix, const std::string &suffix) {
        std::ostringstream oss;
        bool f = false;

        oss << prefix;
        for (const auto &str: list) {
            if (f)
                oss << joiner;
            else
                f = true;
            oss << val_trans(str);
        }
        oss << suffix;

        return oss.str();
    }
    std::string safe_table(const std::string &table, const Type &t) {
        return package_string(table, t.quote_table);
    }
    std::string safe_field(const std::string &field, const Type &t) {
        return package_string(field, t.quote_field);
    }
    std::string safe_value(const std::string &value) {
        return package_string(value, '\'');
    }
    std::string safe_values(const std::initializer_list<std::string> &value, const std::string &joiner) {
        return join(value, joiner, safe_value);
    }
    std::string toUpperCase(std::string str) {
        for (char &c: str) c = static_cast<unsigned char>(std::toupper(static_cast<unsigned char>(c)));
        return str;
    }
    std::string toLowerCase(std::string str) {
        for (char &c: str) c = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(c)));
        return str;
    }

    void hex_stream(std::ostream &os, const unsigned char *const data, const size_t data_len, const size_t buffer_size) {
        static const constexpr char HEX[] = "0123456789ABCDEF";
        std::vector<char> buffer_(buffer_size > 2 ? (buffer_size > data_len * 2 ? data_len * 2 : buffer_size) : 2);
        const auto buffer = buffer_.data();
        size_t buffer_index = 0;

        for (size_t i = 0; i < data_len; ++i) {
            const auto byte = data[i];
            buffer[buffer_index++] = HEX[(byte >> 4) & 0x0F];
            buffer[buffer_index++] = HEX[byte & 0x0F];
            if (buffer_index >= buffer_size - 1) {
                os.write(buffer, buffer_index);
                buffer_index = 0;
            }
        }
        if (buffer_index > 0) os.write(buffer, buffer_index);
    }
}// namespace sqlcpp::str