#pragma once
#include <assert.h>
#include <GeneralComputation/LoopComputing.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
namespace ZCCTools {

#define ERROR "\033[31m"
#define ERROR_END "\033[0m"
#define WARNING "\033[34m"
#define WARNING_END "\033[0m"

/*make_shared：make_shared<T[]>(int len) is not supported by
 * gcc 11, so we define a new one
 */
template <typename _Tp>
inline std::shared_ptr<_Tp> make_shared(int n) {
    using TYPE = typename std::remove_extent<_Tp>::type;
    return std::shared_ptr<_Tp>(new TYPE[n]());
}

template <class T, size_t Dim>
class Array {
    /*friend functions*/
    template <class T1, class T2, size_t D>
    friend Array<std::common_type_t<T1, T2>, D> operator+(
        const Array<T1, D>& array1,
        const Array<T2, D>& array2);

    template <class T1, class T2, size_t D>
    friend Array<std::common_type_t<T1, T2>, D> operator-(
        const Array<T1, D>& array1,
        const Array<T2, D>& array2);

    template <class T1, class T2, size_t D>
    friend Array<T1, D> operator+(const Array<T1, D>& array1, const T2& V);

    template <class T1, class T2, size_t D>
    friend Array<T1, D> operator-(const Array<T1, D>& array1, const T2& V);

   private:
    std::shared_ptr<T[]> buf = nullptr;
    size_t Len = 0;
#ifndef Debug
    size_t EachDim[Dim] = {};
#endif
    template <size_t... Is>
    inline constexpr void PrintEachDim(std::index_sequence<Is...>) {
        auto PrintDim = [](auto i) { std::cout << i << '\t'; };
        (PrintDim(EachDim[Is]), ...);
    }

    inline constexpr size_t CalIndex(std::array<int, Dim>& CartIndex) {
        size_t FlatIndex = 0;
        Loop(
            [&](auto i) constexpr [[gnu::always_inline]] {
                FlatIndex = (CartIndex[i] + FlatIndex) * EachDim[i + 1];
            },
            std::make_index_sequence<Dim - 1>{});
        FlatIndex += CartIndex[Dim - 1];
        return FlatIndex;
    }

    template <class... LOC>
    inline constexpr size_t CalIndex(LOC... loc) {
        std::array<int, Dim> CartIndex = {loc...};
        return CalIndex(CartIndex);
    }

   public:
#ifdef Debug
    size_t EachDim[Dim] = {};
#endif
    /*default constructor*/
    Array() = default;

    /*std::initializer_list*/
    Array(std::initializer_list<T> list) {
        if constexpr (Dim == 1) {
            Len = list.size();
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
            buf = std::make_shared<T[]>(Len);
#else
            buf = ZCCTools::make_shared<T[]>(Len);
#endif
            auto list_ptr = list.begin();
            for (int i = 0; i < Len; i++)
                buf[i] = list_ptr[i];
            EachDim[0] = Len;
        } else {
            std::cout << ERROR
                      << "Error: the std::initializer_list is only for 1D Array"
                      << ERROR_END << std::endl;
            exit(1);
        }
    }

    /*std::initializer_list*/
    Array(std::initializer_list<std::initializer_list<T>> list) {
        if constexpr (Dim == 2) {
            auto Len1 = list.size();
            /*check the sub-list in each dim is the same */
            auto IsSame = true;
            auto IsSameLen = [&](auto i) -> bool {
                return IsSame &&
                       (list.begin()[0].size() == list.begin()[i].size());
            };
            for (int i = 0; i < Len1; i++)
                IsSame = IsSame && IsSameLen(i);
            assert(IsSame);

            /*determin the size of the buf*/
            auto Len2 = list.begin()[0].size();
            Len = Len1 * Len2;
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
            buf = std::make_shared<T[]>(Len);
#else
            buf = ZCCTools::make_shared<T[]>(Len);
#endif
            EachDim[0] = Len1, EachDim[1] = Len2;

            /*copy data to the buf*/
            for (auto list_ptr = list.begin(); list_ptr != list.end();
                 list_ptr++)
                for (auto sub_list_ptr = list_ptr->begin();
                     sub_list_ptr != list_ptr->end(); sub_list_ptr++) {
                    auto index1 = static_cast<int>(list_ptr - list.begin());
                    auto idnex2 =
                        static_cast<int>(sub_list_ptr - list_ptr->begin());
                    std::array<int, 2> index = {index1, idnex2};
                    buf[CalIndex(index)] = *sub_list_ptr;
                }
        } else {
            std::cout
                << ERROR
                << "Error: the std::initializer_list is only for 2D Array "
                << ERROR_END << std::endl;
            exit(1);
        }
    }

    /*constructor*/
    template <class... EACHDIM>
    Array(EACHDIM... each) {
        constexpr bool IsInteger =
            (true && ... && std::is_integral_v<decltype(each)>);
        /*ensure all input is integer*/
        static_assert(IsInteger == true);
        /*ensure the number of input parameters is the same as Dim*/
        static_assert(sizeof...(each) == Dim);

        /*get the length of all elements*/
        auto GetBufLen = [&]() -> size_t { return (1 * ... * each); };
        Len = GetBufLen();
        /*Allocate buf*/
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
        buf = std::make_shared<T[]>(Len);
#else
        buf = ZCCTools::make_shared<T[]>(Len);
#endif

        /*save the dim information to the array EachDim*/
        std::array<int, sizeof...(each)> EACH = {each...};
        Loop([&](auto i) constexpr
             [[gnu::always_inline]] { EachDim[i] = EACH[i]; },
             std::make_index_sequence<sizeof...(each)>{});
    }

    /*copy constructor*/
    template <class T_t, size_t Dim_t>
    Array(const Array<T_t, Dim_t>& array) {
        /*judge whether T_t can be convert into T*/
        constexpr auto IsConvert = std::is_convertible_v<T_t, T>;
        static_assert(IsConvert);
        /*Dim should be same*/
        static_assert(Dim == Dim_t);

        Len = array.GetLen();
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
        buf = std::make_shared<T[]>(Len);
#else
        buf = ZCCTools::make_shared<T[]>(Len);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] = array_ptr[i];
    }

    /*copy constructor*/
    Array(const Array<T, Dim>& array) {
        Len = array.GetLen();
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
        buf = std::make_shared<T[]>(Len);
#else
        buf = ZCCTools::make_shared<T[]>(Len);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] = array_ptr[i];
    }

    /*move constructor*/
    Array(Array&& array) : Len(array.Len) {
        Loop([&](auto i) constexpr
             [[gnu::always_inline]] { EachDim[i] = array.EachDim[i]; },
             std::make_index_sequence<Dim>{});
        buf = array.buf;
        array.buf = nullptr;
    }

    /*deconstructor*/
    ~Array() = default;

    /*define the dimension of the Array*/
    template <class... EACHDIM>
    void Initial(EACHDIM... each) {
        auto IsZero = true;
        Loop([&](auto i) constexpr
             [[gnu::always_inline]] { IsZero = IsZero && EachDim[i] == 0; },
             std::make_index_sequence<Dim>{});
        if (!IsZero) {
            std::cout << ERROR << "Error: The dime of the Array has been set!"
                      << ERROR_END << std::endl;
            exit(1);
        } else Array(each...);
    };

    auto begin() const { return buf.get(); }
    auto end() const { return buf.get() + Len; }

    /*print the dim infomation*/
    void GetDim() {
        std::cout << "Dim:" << Dim << "\nEach Dim:";
        PrintEachDim(std::make_index_sequence<Dim>{});
        std::cout << std::endl;
    }

    /*get Len*/
    size_t GetLen() const { return Len; }

    /*return buf*/
    std::shared_ptr<T[]> Getbuf() const { return buf; }

    /*index the element*/
    template <class... INDEX>
    T& operator()(INDEX... index) {
        constexpr bool IsInteger =
            (true && ... && std::is_integral_v<decltype(index)>);
        /*ensure all input is integer*/
        static_assert(IsInteger == true);
        /*ensure the number of input is the same as Dim*/
        static_assert(sizeof...(index) == Dim);

        std::array<int, Dim> CartDim = {index...};
        auto Flat = CalIndex(CartDim);
#ifdef Debug
        auto IsInRange = true;
        Loop(
            [&](auto i) {
                IsInRange =
                    IsInRange && CartDim[i] < EachDim[i] && CartDim[i] >= 0;
            },
            std::make_index_sequence<Dim>{});
        assert(IsInRange);
        assert(Flat >= 0 && Flat < Len);
#endif
        return buf[Flat];
    }

    /*index the element*/
    T& operator[](size_t Flat) {
#ifdef Debug
        assert(Flat >= 0 && Flat < Len);
#endif
        return buf[Flat];
    }

    /*= operator*/
    template <class T_t>
    Array<T, Dim>& operator=(const Array<T_t, Dim>& array) {
        /*judge whether T_t can be convert into T*/
        constexpr auto IsConvert = std::is_convertible_v<T_t, T>;
        static_assert(IsConvert);

        if (buf) buf.reset();
        Len = array.GetLen();
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
        buf = std::make_shared<T[]>(Len);
#else
        buf = ZCCTools::make_shared<T[]>(Len);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] = array_ptr[i];
        return *this;
    }

    /*= operator*/
    Array<T, Dim>& operator=(const Array<T, Dim>& array) {
        if (this == &array) return *this;
        if (buf) buf.reset();
        Len = array.GetLen();
#if (defined(__GNUC__) && __GNUC__ >= 12) || \
    (defined(__clang__) && __clang_major__ >= 12)
        buf = std::make_shared<T[]>(Len);
#else
        buf = ZCCTools::make_shared<T[]>(Len);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] = array_ptr[i];
        return *this;
    }

    /*+= operator*/
    template <class T_t>
    Array<T, Dim>& operator+=(const Array<T_t, Dim>& array) {
        /*judge whether T_t can be convert into T*/
        constexpr auto IsConvert = std::is_convertible_v<T_t, T>;
        static_assert(IsConvert);
#ifdef Debug
        auto IsDimSame = true;
        auto DimSame = [&](auto i) -> bool {
            return IsDimSame && (EachDim[i] == array.EachDim[i]);
        };

        Loop(DimSame, std::make_index_sequence<Dim>{});
        assert(IsDimSame);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] += array_ptr[i];
        return *this;
    }

    /*+=operator*/
    Array<T, Dim>& operator+=(const Array<T, Dim>& array) {
#ifdef Debug
        auto IsDimSame = true;
        auto DimSame = [&](auto i) -> bool {
            return IsDimSame && (EachDim[i] == array.EachDim[i]);
        };

        Loop(DimSame, std::make_index_sequence<Dim>{});
        assert(IsDimSame);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] += array_ptr[i];
        return *this;
    }

    /*-= operator*/
    template <class T_t>
    Array<T, Dim>& operator-=(const Array<T_t, Dim>& array) {
        /*judge whether T_t can be convert into T*/
        constexpr auto IsConvert = std::is_convertible_v<T_t, T>;
        static_assert(IsConvert);
#ifdef Debug
        auto IsDimSame = true;
        auto DimSame = [&](auto i) -> bool {
            return IsDimSame && (EachDim[i] == array.EachDim[i]);
        };

        Loop(DimSame, std::make_index_sequence<Dim>{});
        assert(IsDimSame);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] -= array_ptr[i];
        return *this;
    }

    /*-=operator*/
    Array<T, Dim>& operator-=(const Array<T, Dim>& array) {
#ifdef Debug
        auto IsDimSame = true;
        auto DimSame = [&](auto i) -> bool {
            return IsDimSame && (EachDim[i] == array.EachDim[i]);
        };

        Loop(DimSame, std::make_index_sequence<Dim>{});
        assert(IsDimSame);
#endif
        auto array_ptr = array.Getbuf();
        for (int i = 0; i < Len; i++)
            buf[i] -= array_ptr[i];
        return *this;
    }

    /*== operator*/
    template <class T_t, size_t Dim_t>
    bool operator==(Array<T_t, Dim_t>& array) {
        /*judge whether T_t can be convert into T*/
        constexpr auto IsSame = std::is_same_v<T, T_t>;
        static_assert(IsSame);
        /*Dim should be same*/
        static_assert(Dim == Dim_t);

        if (Len != array.GetLen()) return false;
        else {
            auto result = true;
            auto array_ptr = array.Getbuf();
            for (int i = 0; i < Len; i++)
                result = result && (buf[i] == array_ptr[i]);
            return result;
        }
    }

    /*assign a specific value to the Array*/
    template <class Type>
    void SetValue(Type value) {
        constexpr auto TypeOK = std::is_convertible_v<Type, T>;
        static_assert(TypeOK);
        for (int i = 0; i < Len; i++)
            buf[i] = value;
    }

    /*realize the map function*/
    template <class Lam>
    void Map(Lam lam) {
        if constexpr (std::is_same_v<void, decltype(lam(buf[0]))>)
            for (int i = 0; i < Len; i++)
                lam(buf[i]);
        else {
            if constexpr (std::is_convertible_v<decltype(lam(buf[0])), T>)
                for (int i = 0; i < Len; i++)
                    buf[i] = lam(buf[i]);
            else {
                std::cerr << ERROR
                          << "Error: the return type of the lambda function "
                             "should be convertile to the type of the Array"
                          << "\n Func:" << __FUNCTION__
                          << "\n File:" << __FILE__ << "\n Line:" << __LINE__
                          << ERROR_END << std::endl;
                exit(1);
            }
        }
    }

    /*Reduce operation*/
    template <class Lam>
    T Reduce(Lam op) {
        T result = op.initial_data;
        for (int i = 0; i < Len; i++)
            result = op(result, buf[i]);
        return result;
    }

    /*Reduce the array within a spercific range*/
    template <class Lam>
    T Reduce(Lam op,
             const std::array<size_t, Dim>& start_p,
             const std::array<size_t, Dim>& end_p) {
        T result = op.initial_data;
        auto op_new = [&](auto... loc) {
            result = op(result, buf[CalIndex(loc...)]);
        };
        DeepLoop(op_new, start_p, end_p);
        return result;
    }

    /*swarp the data of two Arrays*/
    void Swap(Array<T, Dim>& array) {
        if (this == &array) return;
        else {
            assert(buf != nullptr && array.buf != nullptr);
            auto temp = buf;
            buf = array.buf;
            array.buf = temp;
        }
    }
};
}  // namespace ZCCTools