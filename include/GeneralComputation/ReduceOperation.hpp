#pragma once
#include <limits>
#include <type_traits>
namespace ZCCTools {
namespace ReduceOperation {
template <class T>
struct ADD {
    T initial_data = 0;
    T operator()(T a, T b) { return a + b; };
};

template <class T>
struct MUL {
    T initial_data = 1;
    T operator()(T a, T b) { return a * b; };
};

template <class T>
struct MIN {
    T initial_data = std::numeric_limits<T>::max();
    T operator()(T a, T b) { return std::min(a, b); };
};

template <class T>
struct MAX {
    T initial_data = std::numeric_limits<T>::min();
    T operator()(T a, T b) { return std::max(a, b); };
};

};  // namespace ReduceOperation
}  // namespace ZCCTools