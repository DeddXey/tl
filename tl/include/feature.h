#ifndef FEATURE_H
#define FEATURE_H

#include "log/log.h"
#include "utility.h"

#include <type_traits>


template <typename TR>
void printTypeName(UNUSED TR t)
{
    if (std::is_const<TR>::value)
        con.debug() << "const ";
    if (std::is_volatile<TR>::value)
        con.debug() <<" volatile ";
    if (std::is_lvalue_reference<TR>::value)
        con.debug() <<" & ";
    else if (std::is_rvalue_reference<TR>::value)
        con.debug() <<" && ";

}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Array manipulations
// TODO(alex): init by function
///
/// Template of array structure used to init members in compile time
///
template <typename T, int N>
struct InitArray {

    T	data[N];

    T& operator[](const int idx)
    {
        return data[idx];
    }
};

///
/// Service function to array member initialization
///
template <typename EpType, typename S, typename I, S... ints>
constexpr InitArray<EpType, sizeof... (ints)> initArrayMembersConstImpl(I&& val,
                                                                        std::integer_sequence<S, ints...>)
{
    static_assert(std::is_lvalue_reference<decltype (val)>::value, "no lvalue");
    auto l = [](auto&& a, UNUSED auto b)
    {
        static_assert(std::is_lvalue_reference<decltype (a)>::value, "no lvalue");
        return EpType(std::forward<I>(a));
    };

    return { l(val, ints)... };
}

///
/// Main function to array member initialization
///
template <typename EpType, int N, typename I>
constexpr auto initArrayMembersConst(I&& val) -> InitArray<EpType, N>
{
    static_assert(std::is_lvalue_reference<decltype (val)>::value, "no lvalue");
    return initArrayMembersConstImpl<EpType>(std::forward<I>(val),
                                             std::make_integer_sequence<int, N>{});
}




#endif // FEATURE_H
