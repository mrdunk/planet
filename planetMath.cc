#include "planetMath.h"

CartesianCoord ToCartesian(const PolarCoord polar_coord){
#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "C_HEIGHT:\t" << C_HEIGHT(polar_coord) << std::endl;
    std::cout << "C_LATT:\t" << C_LATT(polar_coord) << "\tsin: " << sin(C_LATT(polar_coord)) << "\tcos: " << cos(C_LATT(polar_coord)) << std::endl;
    std::cout << "C_LONG:\t" << C_LONG(polar_coord) << "\tsin: " << sin(C_LONG(polar_coord)) << "\tcos: " << cos(C_LONG(polar_coord)) << std::endl;
#endif
    return CartesianCoord(
        cos(C_LATT(polar_coord)) * sin(C_LONG(polar_coord)) * (WORLD_RADIUS + C_HEIGHT(polar_coord)),
        cos(C_LATT(polar_coord)) * cos(C_LONG(polar_coord)) * (WORLD_RADIUS + C_HEIGHT(polar_coord)),
        sin(C_LATT(polar_coord)) * (WORLD_RADIUS + C_HEIGHT(polar_coord))
    );
}


void DisplayCoord(const CartesianCoord coordinate) {
    std::cout << "x: " << C_X(coordinate) << "\ty: " << C_Y(coordinate) << "\tz: " << C_Z(coordinate) << std::endl;
}

void DisplayPolarCoord(const PolarCoord coordinate) {
    std::cout << "height: " << C_HEIGHT(coordinate) / PI << "\tlattitude: " << 180 * C_LATT(coordinate) / PI << "\tlongditude: " << 180 * C_LONG(coordinate) / PI << std::endl;
}

int DistanceBetween(CartesianCoord c0, CartesianCoord c1) {
        return sqrt((C_X(c0) - C_X(c1)) * (C_X(c0) - C_X(c1)) + (C_Y(c0) - C_Y(c1)) * (C_Y(c0) - C_Y(c1)) + (C_Z(c0) - C_Z(c1)) * (C_Z(c0) - C_Z(c1)));
}
