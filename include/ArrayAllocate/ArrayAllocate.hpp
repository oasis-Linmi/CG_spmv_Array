#ifndef __ARRARALLOCATE_
#define __ARRARALLOCATE_
#include <Array/Array.hpp>
using namespace ZCCTools;
//template <class T>
// T** Allocate2D(int M, int N) {
//     T** tmp;
//     tmp = new T*[M];
//     for (int i = 0; i < M; i++) {
//         tmp[i] = new T[N];
//         /*initialize the value to be 0*/
//         for (int j = 0; j < N; j++)
//             tmp[i][j] = 0;
//     }

//     return tmp;
// }

template <class T>
Array<T, 2> Allocate2D(int M, int N){
    Array<T, 2> tmp(M, N);
    tmp.SetValue(0);
    return tmp;
}

// template <class T>
// void Delte2D(T** buf, int M) {
//     for (int i = 0; i < M; i++)
//         delete[] buf[i];

//     delete[] buf;
// }

template <class T>
void Delte2D(Array<T,2> buf, int M){
    delete &buf;
}

// template <class T>
// T* Allocate1D(int M) {
//     T* tmp = new T[M];
//     /*initialize the value to be 0*/
//     for (int i = 0; i < M; i++)
//         tmp[i] = 0;
//     return tmp;
// }

template <class T>
Array<T, 1> Allocate1D(int M){
    Array<T, 1> tmp(M);
    tmp.SetValue(0);
    return tmp;
}

// template <class T>
// void Delte1D(T* buf) {
//     delete[] buf;
// }

template <class T>
void Delte1D(Array<T, 1> buf){
    delete &buf;
}

#endif