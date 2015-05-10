#ifndef __PLANET_MATH__
#define __PLANET_MATH__


#include <tuple>
#include <math.h>       /* cos */
#include <iostream>

#include "defines.h"


#define C_X(a)  std::get<0>(a)
#define C_Y(a)  std::get<1>(a)
#define C_Z(a)  std::get<2>(a)

#define C_HEIGHT(a) std::get<0>(a)
#define C_LATT(a)   std::get<1>(a)
#define C_LONG(a)   std::get<2>(a)
#define C_LATT_RADINS(a)  (M_PI * 2 * std::get<1>(a) / CIRCLE_DIVISIONS)
#define C_LONG_RADINS(a)  (M_PI * 2 * std::get<2>(a) / CIRCLE_DIVISIONS)

//typedef std::tuple<int, int, int> PolarCoord;
//typedef std::tuple<int, int, int> CartesianCoord;
typedef std::tuple<double, double, double> PolarCoord;
typedef std::tuple<double, double, double> CartesianCoord;

CartesianCoord ToCartesian(const PolarCoord polar_coord);
PolarCoord ToPolar(CartesianCoord cartesian_coord);

void DisplayCoord(const CartesianCoord coordinate);
void DisplayPolarCoord(const PolarCoord coordinate);

double DistanceBetween(CartesianCoord c0, CartesianCoord c1);

#endif  // __PLANET_MATH__
