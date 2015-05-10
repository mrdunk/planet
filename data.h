#ifndef __PLANET_DATA__
#define __PLANET_DATA__


#include <memory>
#include <map>
#include <vector>

#include "defines.h"
#include "planetMath.h"

#define MAX_NEIGHBOURS 6


typedef std::tuple<int, int, int> PointKey;

class Point : public std::enable_shared_from_this<Point> {
  public:
    static std::shared_ptr<Point> GetInstance(const int recursion, PolarCoord coordinate);
    static std::map<PointKey, std::shared_ptr<Point> > all_points_;

    int recursion_;
    int height_;
    int lattitude_;
    int longditude_;
    std::shared_ptr<Point> parent_;
    std::shared_ptr<Point> child_;
    std::shared_ptr<Point> neighbours_[MAX_NEIGHBOURS];
    int neighbours_count_;

    void AddNeighbour(const std::shared_ptr<Point> neighbour);
    bool IsNeighbour(const std::shared_ptr<Point> neighbour) const;
    static PointKey ClampAngle(PointKey coordinate);

    PolarCoord GetPolarCoord() const {
        return PolarCoord {height_, lattitude_, longditude_};
    }

    CartesianCoord GetCartesianCoord() const {
        return ToCartesian(GetPolarCoord());
    }

  private:
    Point();
    Point(const int recursion, const PolarCoord coordinate) : 
                                         recursion_(recursion),
                                         height_(round(std::get<0>(coordinate))),
                                         lattitude_(round(std::get<1>(coordinate))),
                                         longditude_(round(std::get<2>(coordinate))),
                                         neighbours_count_(0) {};
    Point(const int recursion, const double height, const double lattitude, const double longditude) : 
                                         recursion_(recursion),
                                         height_(round(height)),
                                         lattitude_(round(lattitude)),
                                         longditude_(round(longditude)),
                                         neighbours_count_(0) {};
};


class Face {
  public:
    Face(const std::shared_ptr<Point> p0, const std::shared_ptr<Point> p1, const std::shared_ptr<Point> p2) : p0_(p0), p1_(p1), p2_(p2) {};
    std::shared_ptr<Point> p0_;
    std::shared_ptr<Point> p1_;
    std::shared_ptr<Point> p2_;

    inline bool operator!=(const Face& rhs) const {
        return (p0_->lattitude_ != rhs.p0_->lattitude_ || p0_->longditude_ != rhs.p0_->longditude_ ||
                p1_->lattitude_ != rhs.p1_->lattitude_ || p1_->longditude_ != rhs.p1_->longditude_ ||
                p2_->lattitude_ != rhs.p2_->lattitude_ || p2_->longditude_ != rhs.p2_->longditude_);
    };

    void Split();
};

class FaceIterator {
  public:
    FaceIterator(const int target_recursion) : target_recursion_(target_recursion), end_point_(Point::GetInstance(0, PointKey{0,0,0})) {};
    const Face begin() {
        it_neghbours_1_ = 0;
        it_neghbours_2_ = 0;
        it_point_ = Point::all_points_.begin();
        return GetFace();
    }
    Face end() const {
        return Face{end_point_, end_point_, end_point_};
    }
    const Face GetFace();
  private:
    std::map<PointKey, std::shared_ptr<Point> >::iterator it_point_;
    int target_recursion_;
    std::shared_ptr<Point> end_point_;
    int it_neghbours_1_;
    int it_neghbours_2_;
};

void bootstrap();
void bootstrap2(int recursion_to_split);


#endif  // __PLANET_DATA__
