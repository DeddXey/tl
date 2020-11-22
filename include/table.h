#ifndef TABLE_H
#define TABLE_H
///
/// Генерация последовательностей,
/// из темы http://stackoverflow.com/questions/13313980/populate-an-array-using-constexpr-at-compile-time
/// реализация http://cplusadd.blogspot.ru/2013/02/c11-compile-time-lookup-tablearray-with.html
/// объяснение http://loungecpp.net/cpp/indices-trick/

template<unsigned... Is> struct Seq{};

template<unsigned N, unsigned... Is>
struct GenSeq : GenSeq<N-1, N-1, Is...>{};

template<unsigned... Is>
struct GenSeq<0, Is...> : Seq<Is...>{};


template<typename T, unsigned N>
struct Table
{
    T  values[N];
    static constexpr unsigned length = N;

	T& operator[](const int idx)
	{
		return values[idx];
	}
};


template<typename T,  typename LambdaType, unsigned... Is>
constexpr Table<T, sizeof...(Is) > tableGenerator(Seq<Is...>, LambdaType evalFunc)
{
    return { evalFunc(Is)... };
}

template<typename T, unsigned N, typename LambdaType>
constexpr Table<T, N> tableGenerator( LambdaType evalFunc )
{
    return tableGenerator<T>(GenSeq<N>(), evalFunc);
}


//template<typename T,  typename LambdaType, unsigned... Is>
//constexpr T[sizeof... (Is)] arrayGenerator(Seq<Is...>,
//                                           LambdaType evalFunc)
//{
//    return { evalFunc(Is)... };
//}

//template<typename T, unsigned N, typename LambdaType>
//constexpr auto arrayGenerator( LambdaType evalFunc )
//{
//    return tableGenerator<T>(GenSeq<N>(), evalFunc);
//}






#endif // TABLE_H
