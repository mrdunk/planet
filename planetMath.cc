#include "planetMath.h"

CartesianCoord ToCartesian(PolarCoord polar_coord){
#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "C_RADIUS:\t" << C_RADIUS(polar_coord) << std::endl;
    std::cout << "C_LATT:\t" << C_LATT(polar_coord) << "\tsin: " << sin(C_LATT(polar_coord)) << "\tcos: " << cos(C_LATT(polar_coord)) << std::endl;
    std::cout << "C_LONG:\t" << C_LONG(polar_coord) << "\tsin: " << sin(C_LONG(polar_coord)) << "\tcos: " << cos(C_LONG(polar_coord)) << std::endl;
#endif
    return CartesianCoord(
        cos(C_LATT(polar_coord)) * sin(C_LONG(polar_coord)) * C_RADIUS(polar_coord),
        cos(C_LATT(polar_coord)) * cos(C_LONG(polar_coord)) * C_RADIUS(polar_coord),
        sin(C_LATT(polar_coord)) * C_RADIUS(polar_coord)
    );
}


void DisplayCoord(CartesianCoord coordinate){
    std::cout << "x: " << C_X(coordinate) << "\ty: " << C_Y(coordinate) << "\tz: " << C_Z(coordinate) << std::endl;
}

void DisplayPolarCoord(PolarCoord coordinate) {
    std::cout << "radius: " << 180 * C_RADIUS(coordinate) / PI << "\tlattitude: " << 180 * C_LATT(coordinate) / PI << "\tlongditude: " << 180 * C_LONG(coordinate) / PI << std::endl;
}

