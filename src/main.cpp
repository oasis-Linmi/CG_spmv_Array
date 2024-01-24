
#include <omp.h>
#include <Setting.hpp>
#include <iostream>
#include <Array/Array.hpp>
using namespace ZCCTools;
void initialization();
void finalization();
void assemble();
void CG(Array<double,2> A,
        double* x,
        Array<double,1> b,
        unsigned int Xnode,
        unsigned int Ynode);
void Output();

int main() {
    initialization();
    assemble();

    auto t0 = omp_get_wtime();
    CG(A, x, b, Xnode, Ynode);
    auto t1 = omp_get_wtime();

    Output();
    std::cout << "Runing time is " << t1 - t0 << "s" << std::endl;
    finalization();
    return 0;
}