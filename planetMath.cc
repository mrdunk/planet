#include "planetMath.h"

CartesianCoord ToCartesian(const PolarCoord polar_coord){
#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "C_HEIGHT:\t" << C_HEIGHT(polar_coord) << std::endl;
    std::cout << "C_LATT:\t" << C_LATT(polar_coord) << "\tsin: " << sin(C_LATT(polar_coord)) << "\tcos: " << cos(C_LATT(polar_coord)) << std::endl;
    std::cout << "C_LONG:\t" << C_LONG(polar_coord) << "\tsin: " << sin(C_LONG(polar_coord)) << "\tcos: " << cos(C_LONG(polar_coord)) << std::endl;
#endif
    return CartesianCoord(
        (cos(C_LATT_RADINS(polar_coord)) * sin(C_LONG_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS)),
        (cos(C_LATT_RADINS(polar_coord)) * cos(C_LONG_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS)),
        (sin(C_LATT_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS))
        //round(cos(C_LATT_RADINS(polar_coord)) * sin(C_LONG_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS)),
        //round(cos(C_LATT_RADINS(polar_coord)) * cos(C_LONG_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS)),
        //round(sin(C_LATT_RADINS(polar_coord)) * (C_HEIGHT(polar_coord) + WORLD_RADIUS))
    );
}

PolarCoord ToPolar(CartesianCoord cartesian_coord){
    double lattitude, longditude;

    lattitude = asin(C_Z(cartesian_coord) / (sqrt((C_X(cartesian_coord)*C_X(cartesian_coord)) + 
              (C_Y(cartesian_coord)*C_Y(cartesian_coord)) + (C_Z(cartesian_coord)*C_Z(cartesian_coord))))) / (M_PI *2);

    if(abs(C_X(cartesian_coord)) > 0){
        longditude = 0.25 - atan(C_Y(cartesian_coord) / C_X(cartesian_coord)) / (M_PI *2);
        if(C_X(cartesian_coord) < 0){
            longditude += 0.5;
        }
    } else {
        longditude = atan(C_X(cartesian_coord) / C_Y(cartesian_coord)) / (M_PI *2);
        if(C_Y(cartesian_coord) < 0){
            longditude += 0.5;
        }
    }

    while(longditude < 0){
        longditude += 1;
    }
    while(longditude > 1){
        longditude -= 1;
    }

    //return PolarCoord(0, round(lattitude * CIRCLE_DIVISIONS), round(longditude * CIRCLE_DIVISIONS));
    return PolarCoord(0, (lattitude * CIRCLE_DIVISIONS), (longditude * CIRCLE_DIVISIONS));
}


void DisplayCoord(const CartesianCoord coordinate) {
    std::cout << "x: " << C_X(coordinate) << "\ty: " << C_Y(coordinate) << "\tz: " << C_Z(coordinate) << std::endl;
}

void DisplayPolarCoord(const PolarCoord coordinate) {
    //std::cout << "height: " << C_HEIGHT(coordinate) / PI << "\tlattitude: " << 360 * C_LATT(coordinate) / PI << "\tlongditude: " << 360 * C_LONG(coordinate) / PI << std::endl;
    std::cout << "height: " << std::get<0>(coordinate) << "\tlattitude: " << std::get<1>(coordinate) << "\tlongditude: " << std::get<2>(coordinate) << std::endl;
}

double DistanceBetween(CartesianCoord c0, CartesianCoord c1) {
        return sqrt((C_X(c0) - C_X(c1)) * (C_X(c0) - C_X(c1)) + (C_Y(c0) - C_Y(c1)) * (C_Y(c0) - C_Y(c1)) + (C_Z(c0) - C_Z(c1)) * (C_Z(c0) - C_Z(c1)));
}
