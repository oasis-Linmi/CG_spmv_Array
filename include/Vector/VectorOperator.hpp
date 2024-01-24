#ifndef _VECTOROPERATOR_
#define _VECTOROPERATOR_
#include <utility>

template <class Type, class OP, class... VEC>
void VectorOperation(OP op, unsigned L, Type result, VEC... vec) {
    for (int i = 0; i < L; i++){
        result[i] = op(i, vec...);
        // printf("%d ", i);
    }

}

template <class Type>
Type VectorDotVector(Array<Type,1> a, Array<Type,1> b, unsigned int L) {
    Type sum = 0;

    for (int i = 0; i < L; i++)
        sum += a[i] * b[i];
        
    return sum;
}

template <class Type>
Type VectorDotVector(double* a, Array<Type,1> b, unsigned int L) {
    Type sum = 0;

    for (int i = 0; i < L; i++)
        sum += a[i] * b[i];
        
    return sum;
}

template <class Type>
Type VectorNorm(Array<Type,1> a, unsigned int L) {
    return VectorDotVector(a, a, L);
}

#endif