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

    coordinate = ClampAngle(coordinate);
    PointKey key(recursion, std::get<1>(coordinate), std::get<2>(coordinate));

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
        new_point_high = Point::GetInstance(0, PolarCoord(0, CIRCLE_DIVISIONS * .08, p));
        root_node->AddNeighbour(new_point_high);
        if(last_point_high){
            last_point_high->AddNeighbour(new_point_high);
        }

        new_point_low = Point::GetInstance(0, PolarCoord(0, -CIRCLE_DIVISIONS * .08, p - (CIRCLE_DIVISIONS / 10)));
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

void bootstrap2() {
    // TESTING
    std::cout << "Point::all_points_.size(): " << Point::all_points_.size() << std::endl;

    std::vector<Face> new_faces;

    auto it_face = FaceIterator(0);
    Face face = it_face.begin();
    while(face != it_face.end()){
        //face.Split();
        new_faces.push_back(face);
        face = it_face.GetFace();
    }

    for(auto it = new_faces.begin(); it != new_faces.end(); ++it){
        it->Split();
    }
    
    std::cout << "Point::all_points_.size(): " << Point::all_points_.size() << std::endl;
}


void Face::Split(){
    int new_recursion = p0_->recursion_ +1;
    
    PolarCoord new_p0_coord(new_recursion, p0_->lattitude_, p0_->longditude_);
    PolarCoord new_p1_coord(new_recursion, p1_->lattitude_, p1_->longditude_);
    PolarCoord new_p2_coord(new_recursion, p2_->lattitude_, p2_->longditude_);

    PolarCoord new_p01_coord(new_recursion, (p0_->lattitude_ + p1_->lattitude_) /2, (p0_->longditude_ + p1_->longditude_) /2);
    PolarCoord new_p12_coord(new_recursion, (p1_->lattitude_ + p2_->lattitude_) /2, (p1_->longditude_ + p2_->longditude_) /2);
    PolarCoord new_p20_coord(new_recursion, (p2_->lattitude_ + p0_->lattitude_) /2, (p2_->longditude_ + p0_->longditude_) /2);

    std::shared_ptr<Point> new_p0 = Point::GetInstance(new_recursion, new_p0_coord);
    std::shared_ptr<Point> new_p1 = Point::GetInstance(new_recursion, new_p1_coord);
    std::shared_ptr<Point> new_p2 = Point::GetInstance(new_recursion, new_p2_coord);

    std::shared_ptr<Point> new_p01 = Point::GetInstance(new_recursion, new_p01_coord);
    std::shared_ptr<Point> new_p12 = Point::GetInstance(new_recursion, new_p12_coord);
    std::shared_ptr<Point> new_p20 = Point::GetInstance(new_recursion, new_p20_coord);


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
    
    std::cout << "new_p0->neighbours_count_: " << new_p0->neighbours_count_ << std::endl;
}


const Face FaceIterator::GetFace(){
    std::cout << "FaceIterator::GetFace()" << std::endl;
    std::shared_ptr<Point> p1, p2, p3;
    
    while(it_point_ != Point::all_points_.end()){
        std::cout << ".";
        p1 = it_point_->second;

        if(p1->recursion_ == target_recursion_){
            std::cout << "+";
            for( ; it_neghbours_1_ < p1->neighbours_count_; ++it_neghbours_1_){

                // Point up triangle.
                if(p1->neighbours_[it_neghbours_1_] && p1->neighbours_[it_neghbours_1_]->lattitude_ < p1->lattitude_){
                    p2 = p1->neighbours_[it_neghbours_1_];
                    std::cout << "1";
                    // p2 is below first Point.
                    if(p1->lattitude_ == CIRCLE_DIVISIONS/4 ||   // When lattitude_ is at top of globe, we can't compare longditudes.
                            (p1->longditude_ - p2->longditude_ >= 0 && 
                            p1->longditude_ - p2->longditude_ < CIRCLE_DIVISIONS /2) ||
                            (p1->longditude_ - p2->longditude_ < -CIRCLE_DIVISIONS /2)){
                        // p2 is to the left of p1.
                        for( ; it_neghbours_2_ < p2->neighbours_count_; ++it_neghbours_2_){
                            if(p2->neighbours_[it_neghbours_2_] && p2->neighbours_[it_neghbours_2_]->lattitude_ == p2->lattitude_){
                                std::cout << "2";
                                // Second and third Points are at the same lattitude.
                                p3 = p2->neighbours_[it_neghbours_2_];
                                if((p3->longditude_ - p2->longditude_ >= 0 &&                            
                                        p3->longditude_ - p2->longditude_ < CIRCLE_DIVISIONS /2) ||
                                        (p3->longditude_ - p2->longditude_ < -CIRCLE_DIVISIONS /2)){
                                    // p3 is right of p2.
                                    if(p1->IsNeighbour(p3)){
                                        // p1 and p3 are neighbours so p1, p2 & p3 form a Face.
                                        std::cout << "* " << p1->lattitude_ << "," << p1->longditude_ << "\t" <<
                                            p2->lattitude_ << "," << p2->longditude_ << "\t" <<
                                            p3->lattitude_ << "," << p3->longditude_ << "\t" <<
                                            std::endl;
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
                    if(p1->lattitude_ == -CIRCLE_DIVISIONS/4 ||   // When lattitude_ is at bottom of globe, we can't compare longditudes.
                            (p2->longditude_ - p1->longditude_ >= 0 && 
                            p2->longditude_ - p1->longditude_ < CIRCLE_DIVISIONS /2) ||
                            (p2->longditude_ - p1->longditude_ < -CIRCLE_DIVISIONS /2)){
                        // p2 is to the left of p1.
                        for( ; it_neghbours_2_ < p2->neighbours_count_; ++it_neghbours_2_){
                            if(p2->neighbours_[it_neghbours_2_] && p2->neighbours_[it_neghbours_2_]->lattitude_ == p2->lattitude_){
                                // Second and third Points are at the same lattitude.
                                p3 = p2->neighbours_[it_neghbours_2_];
                                if((p2->longditude_ - p3->longditude_ >= 0 &&
                                        p2->longditude_ - p3->longditude_ < CIRCLE_DIVISIONS /2) ||
                                        (p2->longditude_ - p3->longditude_ < -CIRCLE_DIVISIONS /2)){
                                    // p3 is right od p2.
                                    if(p1->IsNeighbour(p3)){
                                        // p1 and p3 are neighbours so p1, p2 & p3 form a Face.
                                        std::cout << "* " << p1->lattitude_ << "," << p1->longditude_ << "\t" <<
                                            p2->lattitude_ << "," << p2->longditude_ << "\t" <<
                                            p3->lattitude_ << "," << p3->longditude_ << "\t" <<
                                            std::endl;
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
