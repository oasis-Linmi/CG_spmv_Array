#include <Array/Array.hpp>
#include <utility>

namespace ZCCTools {
/*Array+Array*/
template <class T1, class T2, size_t D>
Array<std::common_type_t<T1, T2>, D> operator+(const Array<T1, D>& array1,
                                               const Array<T2, D>& array2) {
#ifdef Debug
    assert(array1.Len == array2.Len);
    auto IsDimSame = true;
    auto DimSame = [&](auto i) -> bool {
        return IsDimSame && (array1.EachDim[i] == array2.EachDim[i]);
    };

    Loop(DimSame, std::make_index_sequence<D>{});
    assert(IsDimSame);
#endif

    Array<std::common_type_t<T1, T2>, D> result;
    auto SetDim = [&](auto i) { result.EachDim[i] = array1.EachDim[i]; };
    Loop(SetDim, std::make_index_sequence<D>{});

    auto len = array1.Len;
    result.Len = len;
    result.buf = ZCCTools::make_shared<std::common_type_t<T1, T2>[]>(len);
    for (int i = 0; i < len; i++)
        result.buf[i] = array1.buf[i] + array2.buf[i];

    return result;
}

/*Array+value*/
template <class T1, class T2, size_t D>
Array<T1, D> operator+(const Array<T1, D>& array1, const T2& V) {
    static_assert(std::is_convertible_v<T2, T1>);
    Array<T1, D> result;
    auto len = array1.Len;
    result.Len = len;
    result.buf = new T1[len];
    for (int i = 0; i < len; i++)
        result.buf[i] = array1.buf[i] + V;

    return result;
}

/*Array-Array*/
template <class T1, class T2, size_t D>
Array<std::common_type_t<T1, T2>, D> operator-(const Array<T1, D>& array1,
                                               const Array<T2, D>& array2) {
#ifdef Debug
    assert(array1.Len == array2.Len);
    auto IsDimSame = true;
    auto DimSame = [&](auto i) -> bool {
        return IsDimSame && (array1.EachDim[i] == array2.EachDim[i]);
    };

    Loop(DimSame, std::make_index_sequence<D>{});
    assert(IsDimSame);
#endif

    Array<std::common_type_t<T1, T2>, D> result;
    auto SetDim = [&](auto i) { result.EachDim[i] = array1.EachDim[i]; };
    Loop(SetDim, std::make_index_sequence<D>{});

    auto len = array1.Len;
    result.Len = len;
    result.buf = ZCCTools::make_shared<std::common_type_t<T1, T2>[]>(len);
    for (int i = 0; i < len; i++)
        result.buf[i] = array1.buf[i] - array2.buf[i];

    return result;
}

/*Array-value*/
template <class T1, class T2, size_t D>
Array<T1, D> operator-(const Array<T1, D>& array1, const T2& V) {
    static_assert(std::is_convertible_v<T2, T1>);
    Array<T1, D> result;
    auto len = array1.Len;
    result.Len = len;
    result.buf = new T1[len];
    for (int i = 0; i < len; i++)
        result.buf[i] = array1.buf[i] - V;

    return result;
}

/*realize the map function  when more than one arrays particle in
cpmputation*/
template <class Lam, class... ARRAY>
void Map(Lam lam, ARRAY&&... array) {
    if constexpr (std::is_same_v<void, decltype(lam(0, array...))>) {
        auto ALLARRAY = std::make_tuple(std::ref(array)...);
        auto len = std::get<0>(ALLARRAY).GetLen();
#ifdef Debug
        auto IsSameLen = (true && ... && (array.GetLen() == len));
        assert(IsSameLen);
#endif
        for (int i = 0; i < len; i++)
            lam(i, array...);
    } else {
        std::cerr << "the return type of the lambda function should be void"
                  << "\n Func:" << __FUNCTION__ << "\n File:" << __FILE__
                  << "\n Line:" << __LINE__ << std::endl;
    }
}

/*realize the map function  when more than one arrays particle in cpmputation*/
template <size_t Dim, class Lam, class... ARRAY>
void Map(Lam lam,
         std::array<size_t, Dim> start_p,
         std::array<size_t, Dim> end_p,
         ARRAY&&... array) {
    DeepLoop(lam, start_p, end_p, std::forward<decltype(array)>(array)...);
}
}  // namespace ZCCTools
