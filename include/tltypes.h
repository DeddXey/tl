#ifndef TLTYPES_H
#define TLTYPES_H

#include <utility>
#include <cstdint>

namespace tl {

///
/// Type wrapper class to simplify reloading functions and operators
///
template <typename T>
class TypeWrapper {
    T value;

public:
    TypeWrapper() : value(0) {}
    explicit TypeWrapper(T& val) : value(val) {}
    TypeWrapper(TypeWrapper<T>& rhs) : value(rhs.value) {}
    TypeWrapper(TypeWrapper<T>&& lhs) noexcept : value(std::move(lhs.value))
    {}
    ~TypeWrapper() {}

    T getValue() const;
    void setValue(const T &value);
};

template <typename T>
void TypeWrapper<T>::setValue(const T &val)
{
    value = val;
}

template <typename T>
T TypeWrapper<T>::getValue() const
{
return value;
}



//############################################################################

using Int32T = TypeWrapper<int32_t>;
using Uint32T = TypeWrapper<uint32_t>;
using Float = TypeWrapper<float>;
using Double = TypeWrapper<double>;

}

#endif // TMTYPES_H
