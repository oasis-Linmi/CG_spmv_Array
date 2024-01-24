#ifndef _SPMV_
#define _SPMV_
#include <Array/Array.hpp>
using namespace ZCCTools;
template <class T>
void SpMV(Array<T, 2> A, Array<T, 1> V, Array<T, 1> result, unsigned int M, unsigned int Xnode) {
    for (int i = 0; i < M; i++) {
        result[i] = A(0, i) * V(i) + A(1, i) * V[i - 1 + Xnode] +
                    A(2, i) * V[i + Xnode] + A(3, i) * V[i + 1 + Xnode] +
                    A(4, i) * V[i + Xnode + Xnode];
    }
}

template <class T>
void SpMV(Array<T, 2> A, double* V, Array<T, 1> result, unsigned int M, unsigned int Xnode) {
    for (int i = 0; i < M; i++) {
        result[i] = A(0, i) * V[i] + A(1, i) * V[i - 1 + Xnode] +
                    A(2, i) * V[i + Xnode] + A(3, i) * V[i + 1 + Xnode] +
                    A(4, i) * V[i + Xnode + Xnode];
    }
}
#endif