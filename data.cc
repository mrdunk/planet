#include "data.h"
#include <iostream>


std::map<PointKey, std::shared_ptr<Point> > Point::all_points_;

PointKey Point::ClampAngle(PointKey coordinate){
    int recursion = std::get<0>(coordinate);
    int lattitude = std::get<1>(coordinate);
    int longditude = std::get<2>(coordinate);

    while(lattitude > CIRCLE_DIVISIONS /4){
        lattitude = CIRCLE_DIVISIONS /4;
    }
    while(lattitude < -CIRCLE_DIVISIONS /4){
        lattitude = -CIRCLE_DIVISIONS /4;
    }

    while(longditude >= CIRCLE_DIVISIONS){
        longditude -= CIRCLE_DIVISIONS;
    }
    while(longditude < 0){
        longditude += CIRCLE_DIVISIONS;
    }

    return PointKey(recursion, lattitude, longditude);
}

std::shared_ptr<Point> Point::GetInstance(const int recursion, PolarCoord coordinate){
    // TODO Lock Point::all_points_ so this is threadsafe.

    PointKey key(recursion, round(std::get<1>(coordinate)), round(std::get<2>(coordinate)));
    key = ClampAngle(key);

    auto pointer = all_points_.find(key);
    if(pointer != all_points_.end()){
        // Found a cached Point.
        return pointer->second;
    }

    // Make new Point.
    std::shared_ptr<Point> return_value(new Point(recursion, coordinate));
    all_points_[key] = return_value;
    return return_value;
}

void Point::AddNeighbour(const std::shared_ptr<Point> neighbour){
    if(!neighbour){
        std::cerr << "Invalid neighbour." << std::endl;
        return;
    }

    for(int i = 0; i < MAX_NEIGHBOURS; ++i){
        //std::cout << "i: " << i << "\t\t" << lattitude_ << "\t" << longditude << std::endl;
        if(neighbours_[i] && neighbours_[i]->lattitude_ == neighbour->lattitude_ &&
                neighbours_[i]->longditude_ == neighbour->longditude_){
            // Already has this neighbour registered.
            break;
        }
        if(!neighbours_[i]){
            for(int j = 0; j < MAX_NEIGHBOURS; ++j){
                //std::cout << " j: " << j << "\t\t" << 
                //        neighbour->lattitude_ << "\t" << neighbour->longditude << std::endl;
                if(!neighbour->neighbours_[j]){
                    neighbour->neighbours_[j] = shared_from_this();
                    neighbour->neighbours_count_ = j +1;
                    break;
                }
            }
            neighbours_[i] = std::move(neighbour);
            neighbours_count_ = i +1;
            //neighbours_[i]->AddNeighbour(shared_from_this()); // This is an alternative to the for(j) above.
            break;
        }
    }

}

bool Point::IsNeighbour(const std::shared_ptr<Point> neighbour) const {
    for(int i = 0; i < MAX_NEIGHBOURS; ++i){
        if(!neighbours_[i]){
            return false;
        }
        if(neighbours_[i].get() == neighbour.get()){
            return true;
        }
    }
    return false;
}


void bootstrap() {
    std::shared_ptr<Point> root_node = Point::GetInstance(0, PolarCoord(0, CIRCLE_DIVISIONS /4, 0));
    std::shared_ptr<Point> bottom_node = Point::GetInstance(0, PolarCoord(0, -CIRCLE_DIVISIONS /4, 0));

    std::shared_ptr<Point> new_point_high(nullptr), new_point_low(nullptr), first_point_high(nullptr), first_point_low(nullptr),
            last_point_high(nullptr), last_point_low(nullptr);

    for(int p = 0; p < CIRCLE_DIVISIONS; p += CIRCLE_DIVISIONS / 5){
        new_point_high = Point::GetInstance(0, PolarCoord(0, CIRCLE_DIVISIONS * .075, p));
        root_node->AddNeighbour(new_point_high);
        if(last_point_high){
            last_point_high->AddNeighbour(new_point_high);
        }

        new_point_low = Point::GetInstance(0, PolarCoord(0, -CIRCLE_DIVISIONS * .075, p - (CIRCLE_DIVISIONS / 10)));
        bottom_node->AddNeighbour(new_point_low);
        if(last_point_low){
            last_point_low->AddNeighbour(new_point_low);
        }

        if(last_point_high){
            last_point_high->AddNeighbour(new_point_low);
        }
        new_point_high->AddNeighbour(new_point_low);



        if(!first_point_high){
            first_point_high = new_point_high;
        }
        if(!first_point_low){
            first_point_low = new_point_low;
        }

        last_point_high = new_point_high;
        last_point_low = new_point_low;
    }

    last_point_high->AddNeighbour(first_point_high);
    last_point_low->AddNeighbour(first_point_low);

    last_point_high->AddNeighbour(first_point_low);
}

/*void bootstrap() {
    std::shared_ptr<Point> root_node = Point::GetInstance(0, PolarCoord(0, CIRCLE_DIVISIONS /4, 0));
    std::shared_ptr<Point> bottom_node = Point::GetInstance(0, PolarCoord(0, -CIRCLE_DIVISIONS /4, 0));
    std::shared_ptr<Point> p0_node = Point::GetInstance(0, PolarCoord(0, 0, CIRCLE_DIVISIONS /4));
    std::shared_ptr<Point> p1_node = Point::GetInstance(0, PolarCoord(0, 0, CIRCLE_DIVISIONS /2));
    std::shared_ptr<Point> p2_node = Point::GetInstance(0, PolarCoord(0, 0, 3 * CIRCLE_DIVISIONS /4));
    std::shared_ptr<Point> p3_node = Point::GetInstance(0, PolarCoord(0, 0, 0));

    root_node->AddNeighbour(p0_node);
    root_node->AddNeighbour(p1_node);
    root_node->AddNeighbour(p2_node);
    root_node->AddNeighbour(p3_node);
    
    bottom_node->AddNeighbour(p0_node);
    bottom_node->AddNeighbour(p1_node);
    bottom_node->AddNeighbour(p2_node);
    bottom_node->AddNeighbour(p3_node);

    p0_node->AddNeighbour(p1_node);
    p1_node->AddNeighbour(p2_node);
    p2_node->AddNeighbour(p3_node);
    p3_node->AddNeighbour(p0_node);
}*/

void bootstrap2(int recursion_to_split) {
    // TESTING
    std::cout << "Point::all_points_.size(): " << Point::all_points_.size() << std::endl;

    std::vector<Face> new_faces;

    auto it_face = FaceIterator(recursion_to_split);
    Face face = it_face.begin();
    while(face != it_face.end()){
        new_faces.push_back(face);
        face = it_face.GetFace();
    }

    for(auto it = new_faces.begin(); it != new_faces.end(); ++it){
        it->Split();
    }
    
    std::cout << "Point::all_points_.size(): " << Point::all_points_.size() << std::endl;
}

PolarCoord AveragePoints(PolarCoord p0, PolarCoord p1){
    int average_lattitude, average_longditude;

    if(std::get<1>(p0) == std::get<1>(p1)){
        average_lattitude = std::get<1>(p0);
    } else {
        double distance_between = std::get<1>(p0) - std::get<1>(p1);
        average_lattitude = std::get<1>(p1) + distance_between /2;
//        average_lattitude -= sin(M_PI * 2.0 * average_lattitude / CIRCLE_DIVISIONS) * WORLD_RADIUS / 4;

        //average_lattitude = (std::get<1>(p0) + std::get<1>(p1)) /2;
    }

    if(abs(std::get<1>(p0)) == CIRCLE_DIVISIONS /4){
        average_longditude = std::get<2>(p1);
    } else if(abs(std::get<1>(p1)) == CIRCLE_DIVISIONS /4){
        average_longditude = std::get<2>(p0);
    } else if(abs(std::get<2>(p0) - std::get<2>(p1)) > CIRCLE_DIVISIONS /2){
        average_longditude = (std::get<2>(p0) + std::get<2>(p1) + CIRCLE_DIVISIONS) /2;
    } else {
        average_longditude = (std::get<2>(p0) + std::get<2>(p1)) /2;
    }

    return PolarCoord(0, average_lattitude, average_longditude);
}

CartesianCoord AveragePointsCartesian(CartesianCoord p0, CartesianCoord p1){
    return CartesianCoord((C_X(p0) + C_X(p1)) /2, (C_Y(p0) + C_Y(p1)) /2, (C_Z(p0) + C_Z(p1)) /2);
}


void Face::Split(){
    int new_recursion = p0_->recursion_ +1;
    
    PolarCoord new_p0_coord(0, p0_->lattitude_, p0_->longditude_);
    PolarCoord new_p1_coord(0, p1_->lattitude_, p1_->longditude_);
    PolarCoord new_p2_coord(0, p2_->lattitude_, p2_->longditude_);

    std::shared_ptr<Point> new_p0 = Point::GetInstance(new_recursion, new_p0_coord);
    std::shared_ptr<Point> new_p1 = Point::GetInstance(new_recursion, new_p1_coord);
    std::shared_ptr<Point> new_p2 = Point::GetInstance(new_recursion, new_p2_coord);


    /*PolarCoord new_p01_coord = AveragePoints(p0_->GetPolarCoord(), p1_->GetPolarCoord());
    PolarCoord new_p12_coord = AveragePoints(p1_->GetPolarCoord(), p2_->GetPolarCoord());
    PolarCoord new_p20_coord = AveragePoints(p2_->GetPolarCoord(), p0_->GetPolarCoord());

    std::shared_ptr<Point> new_p01 = Point::GetInstance(new_recursion, new_p01_coord);
    std::shared_ptr<Point> new_p12 = Point::GetInstance(new_recursion, new_p12_coord);
    std::shared_ptr<Point> new_p20 = Point::GetInstance(new_recursion, new_p20_coord);*/

    CartesianCoord new_p01_cartesian = AveragePointsCartesian(p0_->GetCartesianCoord(), p1_->GetCartesianCoord());
    CartesianCoord new_p12_cartesian = AveragePointsCartesian(p1_->GetCartesianCoord(), p2_->GetCartesianCoord());
    CartesianCoord new_p20_cartesian = AveragePointsCartesian(p2_->GetCartesianCoord(), p0_->GetCartesianCoord());

    PolarCoord new_p01_polar = ToPolar(new_p01_cartesian);
    PolarCoord new_p12_polar = ToPolar(new_p12_cartesian);
    PolarCoord new_p20_polar = ToPolar(new_p20_cartesian);

    /*std::cout << std::endl;
    std::cout << "split: ";
    DisplayPolarCoord(new_p0_coord);
    std::cout << "split: ";
    DisplayPolarCoord(new_p1_coord);
    std::cout << "split: ";
    DisplayPolarCoord(new_p2_coord);
    std::cout << "split: ";
    DisplayPolarCoord(new_p01_polar);
    std::cout << "split: ";
    DisplayPolarCoord(new_p12_polar);
    std::cout << "split: ";
    DisplayPolarCoord(new_p20_polar);*/

    std::shared_ptr<Point> new_p01 = Point::GetInstance(new_recursion, new_p01_polar);
    std::shared_ptr<Point> new_p12 = Point::GetInstance(new_recursion, new_p12_polar);
    std::shared_ptr<Point> new_p20 = Point::GetInstance(new_recursion, new_p20_polar);


    p0_->child_ = new_p0;
    new_p0->parent_ = p0_;
    new_p0->AddNeighbour(new_p01);
    new_p0->AddNeighbour(new_p20);

    p1_->child_ = new_p1;
    new_p1->parent_ = p1_;
    new_p1->AddNeighbour(new_p01);
    new_p1->AddNeighbour(new_p12);

    p2_->child_ = new_p2;
    new_p2->parent_ = p2_;
    new_p2->AddNeighbour(new_p12);
    new_p2->AddNeighbour(new_p20);

    new_p01->AddNeighbour(new_p12);
    new_p12->AddNeighbour(new_p20);
    new_p20->AddNeighbour(new_p01);
}


const Face FaceIterator::GetFace(){
    //std::cout << "FaceIterator::GetFace()" << std::endl;
    std::shared_ptr<Point> p1, p2, p3;
    
    while(it_point_ != Point::all_points_.end()){
        p1 = it_point_->second;

        if(p1->recursion_ == target_recursion_){
            for( ; it_neghbours_1_ < p1->neighbours_count_; ++it_neghbours_1_){

                // Point up triangle.
                if(p1->neighbours_[it_neghbours_1_] && p1->neighbours_[it_neghbours_1_]->lattitude_ < p1->lattitude_){
                    p2 = p1->neighbours_[it_neghbours_1_];
                    // p2 is below first Point.
                    if(p1->lattitude_ >= CIRCLE_DIVISIONS/4 ||   // When lattitude_ is at top of globe, we can't compare longditudes.
                            (p1->longditude_ - p2->longditude_ >= 0 && 
                            p1->longditude_ - p2->longditude_ < CIRCLE_DIVISIONS /2) ||
                            (p1->longditude_ - p2->longditude_ < -CIRCLE_DIVISIONS /2)){
                        // p2 is to the left of p1.
                        for( ; it_neghbours_2_ < p2->neighbours_count_; ++it_neghbours_2_){
                            //if(p2->neighbours_[it_neghbours_2_] && abs(p2->neighbours_[it_neghbours_2_]->lattitude_ - p2->lattitude_) < 10.0){
                            {
                                // Second and third Points are at the same lattitude.
                                p3 = p2->neighbours_[it_neghbours_2_];
                                if((p3->longditude_ - p2->longditude_ >= 0 &&                            
                                        p3->longditude_ - p2->longditude_ < CIRCLE_DIVISIONS /2) ||
                                        (p3->longditude_ - p2->longditude_ < -CIRCLE_DIVISIONS /2)){
                                    // p3 is right of p2.
                                    if(p1->IsNeighbour(p3)){
                                        // p1 and p3 are neighbours so p1, p2 & p3 form a Face.
                                        ++it_neghbours_2_;
                                        return Face {p1, p2, p3};
                                    }
                                }
                            }
                        }
                        it_neghbours_2_ = 0;
                    }
                }

                // Point down triangle.
                if(p1->neighbours_[it_neghbours_1_] && p1->neighbours_[it_neghbours_1_]->lattitude_ > p1->lattitude_){
                    p2 = p1->neighbours_[it_neghbours_1_];
                    // p2 is below first Point.
                    if(p1->lattitude_ >= -CIRCLE_DIVISIONS/4 ||   // When lattitude_ is at bottom of globe, we can't compare longditudes.
                            (p2->longditude_ - p1->longditude_ >= 0 && 
                            p2->longditude_ - p1->longditude_ < CIRCLE_DIVISIONS /2) ||
                            (p2->longditude_ - p1->longditude_ < -CIRCLE_DIVISIONS /2)){
                        // p2 is to the left of p1.
                        for( ; it_neghbours_2_ < p2->neighbours_count_; ++it_neghbours_2_){
                            //if(p2->neighbours_[it_neghbours_2_] && abs(p2->neighbours_[it_neghbours_2_]->lattitude_ - p2->lattitude_) < 10.0){
                            {
                                // Second and third Points are at the same lattitude.
                                p3 = p2->neighbours_[it_neghbours_2_];
                                if((p2->longditude_ - p3->longditude_ >= 0 &&
                                        p2->longditude_ - p3->longditude_ < CIRCLE_DIVISIONS /2) ||
                                        (p2->longditude_ - p3->longditude_ < -CIRCLE_DIVISIONS /2)){
                                    // p3 is right of p2.
                                    if(p1->IsNeighbour(p3)){
                                        // p1 and p3 are neighbours so p1, p2 & p3 form a Face.
                                        ++it_neghbours_2_;
                                        return Face {p1, p2, p3};
                                    }
                                }
                            }
                        }
                        it_neghbours_2_ = 0;
                    }
                }
            }
            it_neghbours_1_ = 0;
        }

        ++it_point_;
    }

    return end();
}
