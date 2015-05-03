#ifndef __PLANET_MATH__
#define __PLANET_MATH__


#include <tuple>
#include <math.h>       /* cos */
#include <iostream>

#include "defines.h"

//#define GLM_FORCE_RADIANS
//#include <glm/glm.hpp>
//#include <glm/gtx/vector_angle.hpp>

#define PI 3.14159265

#define C_X(a)  std::get<0>(a)
#define C_Y(a)  std::get<1>(a)
#define C_Z(a)  std::get<2>(a)

#define C_RADIUS(a) std::get<0>(a)
#define C_LATT(a)  (((float)std::get<1>(a) / CIRCLE_DIVISIONS) * PI *2)
#define C_LONG(a)    (((float)std::get<2>(a) / CIRCLE_DIVISIONS) * PI *2)

typedef std::tuple<int, int, int> PolarCoord;
typedef std::tuple<int, int, int> CartesianCoord;

CartesianCoord ToCartesian(PolarCoord);
void DisplayCoord(CartesianCoord coordinate);
void DisplayPolarCoord(PolarCoord coordinate);

#endif  // __PLANET_MATH__
