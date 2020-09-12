#ifndef RANGE_H
#define RANGE_H

#include <type_traits>
#include <utility>


template <typename I1, typename I2>
class Range {

    I1 begin_;
    I2 end_;

public:

    Range(I1 begin, I2 end) :
        begin_(begin), end_(end) {}

    Range(const Range& r) :
        begin_(r.begin_),
        end_(r.end_)
    {

    }


    Range(Range&& r) :
        begin_(std::move(r.begin_)),
        end_(std::move(r.end_))
    {

    }

    constexpr auto begin() const { return begin_; }
    constexpr auto end() const { return end_; }

};

template <typename T>
auto makeRange(T& collection)
{
    return Range<decltype (collection.begin()),
            decltype (collection.end())>
            (collection.begin(), collection.end());
}

template< class T, std::size_t N >
auto makeRange( T (&array)[N] ) noexcept
{
    return Range<T*, T*>(array, array + N);
}


class IteratorStencil {};




template <typename Rng, typename F>
auto operator|(Rng&& rng, F&& c) -> decltype(c(std::forward<Rng>(rng)))
{
    return c(std::forward<Rng>(rng));
}



#endif // RANGE_H
