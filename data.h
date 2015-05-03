#ifndef __PLANET_DATA__
#define __PLANET_DATA__


#include <memory>
#include <map>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "defines.h"
#include "planetMath.h"

#define MAX_NEIGHBOURS 6


typedef class Point : public std::enable_shared_from_this<Point> {
  public:
    static std::shared_ptr<Point> GetInstance(PolarCoord coordinate);
    static std::map<PolarCoord, std::shared_ptr<Point> > all_points_;

    int recursion_;
    int radius_;
    int lattitude_;
    int longditude_;
    std::shared_ptr<Point> parent_;
    std::shared_ptr<Point> child_;
    std::shared_ptr<Point> neighbours_[MAX_NEIGHBOURS];
    int neighbours_count_;

    void AddNeighbour(std::shared_ptr<Point> neighbour);
    bool IsNeighbour(std::shared_ptr<Point> neighbour);
    static int ClampAngle(int value);

  private:
    Point();
    Point(PolarCoord coordinate) : radius_(std::get<0>(coordinate)), lattitude_(std::get<1>(coordinate)), longditude_(std::get<2>(coordinate)) {};
    Point(int radius, int lattitude, int longditude) : radius_(radius), lattitude_(lattitude), longditude_(longditude) {};
} Point;


void bootstrap();


#endif  // __PLANET_DATA__
