#pragma once

enum class parse_result_t {
    unknown,
    ok,
    signature_not_found,
    not_enough_data,
    bad_crc
};

struct parse_error {};

template<typename T>
class parse_result {
public:
    using value_type = T;
private:
    T value_;
    parse_result_t result_ = parse_result_t::unknown;
public:
    explicit parse_result(T val): value_(val), result_(parse_result_t::ok) {}
    explicit parse_result(parse_result_t result): value_(), result_(result) {}

    explicit operator bool() const {
        return result_ == parse_result_t::ok;
    }
    T value() const
    {
        return value_;
    }
    [[nodiscard]] parse_result_t error() const
    {
        return result_;
    }
};

//template<typename T>
//parse_result<T> make_parse_result(T value)
//{
//    return parse_result<T>(value);
//}
//
//parse_result<parse_error> make_parse_result(parse_result_t error)
//{
//    return parse_result<parse_error>(error);
//}