
#include <ArrayAllocate/ArrayAllocate.hpp>
#include <Vector/SpMV.hpp>
#include <Vector/VectorOperator.hpp>
#include <cmath>
#include <iostream>
#include <Array/Array.hpp>
using namespace ZCCTools;
#include <initializer_list>


// Array<double, 1> Array_bias(Array<double, 1> array_in, int bias){
//     double* p_begin = array_in.begin() + bias;
//     double* p_end = array_in.end();
//     std::vector<double> list_p;
//     double* ptr = p_begin;
//     while(1){
//         list_p.push_back(*ptr);
//         if(ptr == p_end){
//             break;
//         }
//         ptr++;
//     }
//     // std::initializer_list<double> initializerList(list_p.begin(), list_p.size());
//     Array<double, 1> ret_array = Array<double, 1>(list_p);
//     return ret_array;
// }
void CG(Array<double,2> A,
        double* x,
        Array<double,1> b,
        unsigned int Xnode,
        unsigned int Ynode) {
    std::cout << "Begin to calculate Ax=b using CG method" << std::endl;

    unsigned int LEN = Xnode * Ynode;

    /*declar paramters*/
    Array<double,1> r = Allocate1D<double>(LEN);
    Array<double,1> r_tmp = Allocate1D<double>(LEN);
    Array<double,1> tmp = Allocate1D<double>(LEN);
    Array<double,1> p_all = Allocate1D<double>(LEN + 2 * Xnode);
    std::cout << "Allocate over" << std::endl;
    // Array<double,1> p = Array_bias(p_all, Xnode);
    double* p = p_all.begin() + Xnode;

    for (int i = 0; i < LEN; i++) {
        r[i] = x[i] = 0;
    }

    /*initializition*/
    /*tmp=A*x0*/
    SpMV<double>(A, x - Xnode, tmp, LEN, Xnode);
    std::cout << "SpMV over" << std::endl;
    /*r0=b-tmp*/
    VectorOperation([](int i, auto a, auto b) { return a[i] - b[i]; }, LEN, r,
                    b, tmp);
    std::cout << "VectorOperation1 over" << std::endl; 
    /*p0=r0*/
    VectorOperation([](int i, auto a) { return a[i]; }, LEN, p, r);
    std::cout << "VectorOperation2 over" << std::endl;
    int iteration = 0;
    while (true) {
        iteration++;

        auto tmp_v = VectorNorm(r, LEN);
        std::cout << "1 over" << std::endl;
        SpMV(A, p - Xnode, tmp, LEN, Xnode);
            std::cout << "2 over" << std::endl;
        auto alpha = tmp_v / VectorDotVector(p, tmp, LEN);
        std::cout << "3 over" << std::endl;
        /* x_k+1=x_k+alpha*p */
        VectorOperation(
            [alpha](int i, auto a, auto b) { return a[i] + alpha * b[i]; }, LEN,
            x, x, p);
        std::cout << "4 over" << std::endl;
        /* r_tmp=r-alpha*tmp */
        VectorOperation(
            [alpha](int i, auto a, auto b) { return a[i] - alpha * b[i]; }, LEN,
            r_tmp, r, tmp);
        std::cout << "5 over" << std::endl;
        auto beta = VectorNorm(r_tmp, LEN) / VectorNorm(r, LEN);
        std::cout << "6 over" << std::endl;
        /* p=r_tmp+beta*p */
        VectorOperation(
            [beta](int i, auto a, auto b) { return a[i] + beta * b[i]; }, LEN,
            p, r_tmp, p);
        std::cout << "7 over" << std::endl;
        /* r_tmp=r */
        VectorOperation([](int i, auto a) { return a[i]; }, LEN, r, r_tmp);
        std::cout << "initializition over" << std::endl;
        /*the loop exit condition*/
        if (iteration % 10 == 0)
            std::cout << "Iterator " << iteration << ": \t the absolute err is "
                      << std::sqrt(VectorNorm(r, LEN)) << std::endl;
        
        if (std::sqrt(VectorNorm(r, LEN)) < 1.e-6) {
            std::cout << "The CG process has been Convergent, and CG method is "
                         "completed!"
                      << std::endl;
            break;
        }
    }

    Delte1D(r);
    Delte1D(r_tmp);
    Delte1D(tmp);
    Delte1D(p_all);
}