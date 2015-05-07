#include "data.h"
#include "gtest/gtest.h"

namespace {

TEST(Data, GetPoints) {
    Point::all_points_.clear();

    std::shared_ptr<Point> root_node = Point::GetInstance(0, PolarCoord(0,0,0));
    std::shared_ptr<Point> another_node = Point::GetInstance(0, PolarCoord(0,0,0));

    EXPECT_EQ(root_node.get(), another_node.get());
    EXPECT_EQ(3, root_node.use_count());  // shared_ptr in use for each of root_node and another_node. Also a copy in Point::all_points_.
}

TEST(Data, PointNeighbours){
    Point::all_points_.clear();

    std::shared_ptr<Point> root_node = Point::GetInstance(0, PolarCoord(0,0,0));

    {
        std::shared_ptr<Point> neighbour1 = Point::GetInstance(0, PolarCoord(0,1,0));
        std::shared_ptr<Point> neighbour2 = Point::GetInstance(0, PolarCoord(0,2,0));
        std::shared_ptr<Point> neighbour3 = Point::GetInstance(0, PolarCoord(0,3,0));
        std::shared_ptr<Point> neighbour4 = Point::GetInstance(0, PolarCoord(0,4,0));
        std::shared_ptr<Point> neighbour5 = Point::GetInstance(0, PolarCoord(0,5,0));

        root_node->AddNeighbour(neighbour1);
        root_node->AddNeighbour(neighbour2);
        root_node->AddNeighbour(neighbour3);
        root_node->AddNeighbour(neighbour4);
        root_node->AddNeighbour(neighbour5);

        // Neghbour relationship should work both ways.
        EXPECT_EQ(root_node.get(), neighbour1->neighbours_[0].get());
        EXPECT_EQ(neighbour1.get(), root_node->neighbours_[0].get());
        EXPECT_EQ(5, root_node->neighbours_count_);
        EXPECT_EQ(1, neighbour1->neighbours_count_);

        neighbour4->AddNeighbour(neighbour5);
        EXPECT_EQ(root_node.get(), neighbour4->neighbours_[0].get());
        EXPECT_EQ(neighbour4.get(), root_node->neighbours_[3].get());
        EXPECT_EQ(root_node.get(), neighbour5->neighbours_[0].get());
        EXPECT_EQ(neighbour5.get(), root_node->neighbours_[4].get());
        EXPECT_EQ(neighbour5.get(), neighbour4->neighbours_[1].get());
        EXPECT_EQ(neighbour4.get(), neighbour5->neighbours_[1].get());
        EXPECT_EQ(2, neighbour4->neighbours_count_);
        EXPECT_EQ(2, neighbour5->neighbours_count_);

        EXPECT_TRUE(neighbour4->IsNeighbour(neighbour5));
        EXPECT_FALSE(neighbour4->IsNeighbour(neighbour3));
    }

    EXPECT_EQ(7, root_node.use_count());  // 1 for "root_node", 5 neighbours and one in Point::all_points_.
    EXPECT_EQ(2, root_node->neighbours_[0].use_count());

    {
        // This should have no affect because a node with the same coordinates has already been added.
        std::shared_ptr<Point> neighbour1 = Point::GetInstance(0, PolarCoord(0,1,0));
        root_node->AddNeighbour(neighbour1);
    }

    EXPECT_EQ(7, root_node.use_count());  // 1 for "root_node", 5 neighbours and one in Point::all_points_.
    EXPECT_EQ(2, root_node->neighbours_[0].use_count());
}

TEST(Data, ClampAngle){
    Point::all_points_.clear();

    PolarCoord coordinate_1(0, 10, 20);
    coordinate_1 = Point::ClampAngle(coordinate_1);
    EXPECT_EQ(10, std::get<1>(coordinate_1));
    EXPECT_EQ(20, std::get<2>(coordinate_1));

    PolarCoord coordinate_2(0, -10, -20);
    coordinate_2 = Point::ClampAngle(coordinate_2);
    EXPECT_EQ(-10, std::get<1>(coordinate_2));
    EXPECT_EQ(CIRCLE_DIVISIONS -20, std::get<2>(coordinate_2));

    PolarCoord coordinate_3(0, CIRCLE_DIVISIONS, 0);
    coordinate_3 = Point::ClampAngle(coordinate_3);
    EXPECT_EQ(CIRCLE_DIVISIONS /4, std::get<1>(coordinate_3));

    PolarCoord coordinate_4(0, -CIRCLE_DIVISIONS, 0);
    coordinate_4 = Point::ClampAngle(coordinate_4);
    EXPECT_EQ(-CIRCLE_DIVISIONS /4, std::get<1>(coordinate_4));

    PolarCoord coordinate_5(0, 0, CIRCLE_DIVISIONS +10);
    coordinate_5 = Point::ClampAngle(coordinate_5);
    EXPECT_EQ(10, std::get<2>(coordinate_5));
}

TEST(Math, ToCartesian){
    const int PARTWAY = 0.707 * WORLD_RADIUS;
    const int HALFWAY = 0.5 * WORLD_RADIUS;


    PolarCoord top_polar(0, CIRCLE_DIVISIONS / 4, 0);
    CartesianCoord cartesian = ToCartesian(top_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(WORLD_RADIUS, C_Z(cartesian));


    PolarCoord top_polar_2(0, CIRCLE_DIVISIONS / 4, CIRCLE_DIVISIONS / 4);
    cartesian = ToCartesian(top_polar_2);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(WORLD_RADIUS, C_Z(cartesian));


    PolarCoord meridian_equator_polar(0, 0, 0);
    cartesian = ToCartesian(meridian_equator_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(WORLD_RADIUS, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord bottom_polar(0, -CIRCLE_DIVISIONS / 4, 0);
    cartesian = ToCartesian(bottom_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(-WORLD_RADIUS, C_Z(cartesian));


    PolarCoord bottom_polar_2(0, -CIRCLE_DIVISIONS / 4, CIRCLE_DIVISIONS / 4);
    cartesian = ToCartesian(bottom_polar_2);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(-WORLD_RADIUS, C_Z(cartesian));


    PolarCoord m90_equator_polar(0, 0, CIRCLE_DIVISIONS / 4);
    cartesian = ToCartesian(m90_equator_polar);

    EXPECT_EQ(WORLD_RADIUS, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord m180_equator_polar(0, 0, CIRCLE_DIVISIONS / 2);
    cartesian = ToCartesian(m180_equator_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(-WORLD_RADIUS, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord m270_equator_polar(0, 0, -CIRCLE_DIVISIONS / 4);
    cartesian = ToCartesian(m270_equator_polar);

    EXPECT_EQ(-WORLD_RADIUS, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord m270_equator_polar_2(0, 0, 3 * CIRCLE_DIVISIONS / 4);
    cartesian = ToCartesian(m270_equator_polar_2);

    EXPECT_EQ(-WORLD_RADIUS, C_X(cartesian));
    EXPECT_EQ(0, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord meridian_45_polar(0, CIRCLE_DIVISIONS / 8, 0);
    cartesian = ToCartesian(meridian_45_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(PARTWAY, C_Y(cartesian));
    EXPECT_EQ(PARTWAY, C_Z(cartesian));


    PolarCoord meridian_n45_polar(0, -CIRCLE_DIVISIONS / 8, 0);
    cartesian = ToCartesian(meridian_n45_polar);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(PARTWAY, C_Y(cartesian));
    EXPECT_EQ(-PARTWAY, C_Z(cartesian));


    PolarCoord meridian_n45_polar_2(0, 7 * CIRCLE_DIVISIONS / 8, 0);
    cartesian = ToCartesian(meridian_n45_polar_2);

    EXPECT_EQ(0, C_X(cartesian));
    EXPECT_EQ(PARTWAY, C_Y(cartesian));
    EXPECT_EQ(-PARTWAY, C_Z(cartesian));


    PolarCoord right45_up0_polar(0, 0, CIRCLE_DIVISIONS / 8);
    cartesian = ToCartesian(right45_up0_polar);

    EXPECT_EQ(PARTWAY, C_X(cartesian));
    EXPECT_EQ(PARTWAY, C_Y(cartesian));
    EXPECT_EQ(0, C_Z(cartesian));


    PolarCoord right45_up45_polar(0, CIRCLE_DIVISIONS / 8, CIRCLE_DIVISIONS / 8);
    cartesian = ToCartesian(right45_up45_polar);

    EXPECT_EQ(HALFWAY, C_X(cartesian));
    EXPECT_EQ(HALFWAY, C_Y(cartesian));
    EXPECT_EQ(PARTWAY, C_Z(cartesian));


    PolarCoord left45_up45_polar(0, CIRCLE_DIVISIONS / 8, -CIRCLE_DIVISIONS / 8);
    cartesian = ToCartesian(left45_up45_polar);

    EXPECT_EQ(-HALFWAY, C_X(cartesian));
    EXPECT_EQ(HALFWAY, C_Y(cartesian));
    EXPECT_EQ(PARTWAY, C_Z(cartesian));


    PolarCoord right45_down45_polar(0, -CIRCLE_DIVISIONS / 8, CIRCLE_DIVISIONS / 8);
    cartesian = ToCartesian(right45_down45_polar);

    EXPECT_EQ(HALFWAY, C_X(cartesian));
    EXPECT_EQ(HALFWAY, C_Y(cartesian));
    EXPECT_EQ(-PARTWAY, C_Z(cartesian));


    PolarCoord left45_down45_polar(0, -CIRCLE_DIVISIONS / 8, -CIRCLE_DIVISIONS / 8);
    cartesian = ToCartesian(left45_down45_polar);

    EXPECT_EQ(-HALFWAY, C_X(cartesian));
    EXPECT_EQ(HALFWAY, C_Y(cartesian));
    EXPECT_EQ(-PARTWAY, C_Z(cartesian));
}

TEST(Data, Bootstrap){
    Point::all_points_.clear();

    bootstrap();

    EXPECT_EQ(12, Point::all_points_.size());

    for(auto it = Point::all_points_.begin(); it!=Point::all_points_.end(); ++it){
        EXPECT_EQ(5, it->second->neighbours_count_);
    }
}

TEST(Data, FaceSplit){
    Point::all_points_.clear();

    std::shared_ptr<Point> p0 = Point::GetInstance(0, PolarCoord(0, 0, 0));
    std::shared_ptr<Point> p1 = Point::GetInstance(0, PolarCoord(0, 0, 20));
    std::shared_ptr<Point> p2 = Point::GetInstance(0, PolarCoord(0, 20, 10));

    p0->AddNeighbour(p1);
    p1->AddNeighbour(p2);
    p2->AddNeighbour(p0);

    Face face(p0, p1, p2);

    EXPECT_EQ(3, Point::all_points_.size());
    EXPECT_EQ(2, p0->neighbours_count_);
    EXPECT_EQ(2, p1->neighbours_count_);
    EXPECT_EQ(2, p2->neighbours_count_);


    face.Split();
    
    EXPECT_EQ(9, Point::all_points_.size());
    EXPECT_EQ(2, p0->neighbours_count_);
    EXPECT_EQ(2, p1->neighbours_count_);
    EXPECT_EQ(2, p2->neighbours_count_);

    std::shared_ptr<Point> p0_1 = Point::GetInstance(1, PolarCoord(0, 0, 0));
    std::shared_ptr<Point> p1_1 = Point::GetInstance(1, PolarCoord(0, 0, 20));
    std::shared_ptr<Point> p2_1 = Point::GetInstance(1, PolarCoord(0, 20, 10));
    std::shared_ptr<Point> p01_1 = Point::GetInstance(1, PolarCoord(0, 0, 10));
    std::shared_ptr<Point> p12_1 = Point::GetInstance(1, PolarCoord(0, 10, 15));
    std::shared_ptr<Point> p20_1 = Point::GetInstance(1, PolarCoord(0, 10, 5));

    EXPECT_EQ(9, Point::all_points_.size());
    EXPECT_EQ(2, p0_1->neighbours_count_);
    EXPECT_EQ(2, p1_1->neighbours_count_);
    EXPECT_EQ(2, p2_1->neighbours_count_);
    EXPECT_EQ(2, p01_1->neighbours_count_);
    EXPECT_EQ(2, p12_1->neighbours_count_);
    EXPECT_EQ(2, p20_1->neighbours_count_);

    EXPECT_TRUE(p0_1->IsNeighbour(p01_1));
    EXPECT_TRUE(p0_1->IsNeighbour(p20_1));
    EXPECT_TRUE(p1_1->IsNeighbour(p01_1));
    EXPECT_TRUE(p1_1->IsNeighbour(p12_1));
    EXPECT_TRUE(p2_1->IsNeighbour(p12_1));
    EXPECT_TRUE(p2_1->IsNeighbour(p20_1));
}


}  // namespace


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
