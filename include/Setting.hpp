#ifndef _SETTING_
#define _SETTING_
#include <Array/Array.hpp>
using namespace ZCCTools;
/*the number of nodes in each x and y directions*/
inline constexpr unsigned int Xnode = 100;
/*the number of nodes in each x and y directions*/
inline constexpr unsigned int Ynode = Xnode;

/*the number of nodes in each x and y directions*/
inline constexpr double Q = 1.0;

/*the range of the problem*/
inline constexpr double R = 1.0;

/*space step*/
inline constexpr double h = R / (Xnode - 1);

/*the 2D pointer used to store the Martix*/
inline Array<double,2> A;

/*the 1D pointer used to store the vector x*/
inline double* x;
inline Array<double,1> x_all;

/*the 1D pointer used to store the vector b*/
inline Array<double,1> b;

#endif
