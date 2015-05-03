#include "data.h"
#include <iostream>


std::map<PolarCoord, std::shared_ptr<Point> > Point::all_points_;

int Point::ClampAngle(int value){
    while(value >= CIRCLE_DIVISIONS){
        value -= CIRCLE_DIVISIONS;
    }
    while(value < 0){
        value += CIRCLE_DIVISIONS;
    }

    return value;
}

std::shared_ptr<Point> Point::GetInstance(PolarCoord coordinate){
    // TODO Lock Point::all_points_ so this is threadsafe.

    std::get<1>(coordinate) = Point::ClampAngle(std::get<1>(coordinate));
    std::get<2>(coordinate) = Point::ClampAngle(std::get<2>(coordinate));

    auto pointer = all_points_.find(coordinate);
    if(pointer != all_points_.end()){
        // Found a cached Point.
        return pointer->second;
    }

    // Make new Point.
    std::shared_ptr<Point> return_value(new Point(coordinate));
    all_points_[coordinate] = return_value;
    return return_value;
}

void Point::AddNeighbour(std::shared_ptr<Point> neighbour){
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

bool Point::IsNeighbour(std::shared_ptr<Point> neighbour){
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

void bootstrap2() {
    std::shared_ptr<Point> root_node = Point::GetInstance(PolarCoord(0, 0, 0));
    std::shared_ptr<Point> right_node = Point::GetInstance(PolarCoord(WORLD_RADIUS, 0, CIRCLE_DIVISIONS /4));
    std::shared_ptr<Point> forward_node = Point::GetInstance(PolarCoord(WORLD_RADIUS, 0, 0));
    std::shared_ptr<Point> top_node = Point::GetInstance(PolarCoord(WORLD_RADIUS, CIRCLE_DIVISIONS /4, 0));

    root_node->AddNeighbour(right_node);
    root_node->AddNeighbour(top_node);
    root_node->AddNeighbour(forward_node);

    right_node->AddNeighbour(top_node);
    right_node->AddNeighbour(forward_node);

    top_node->AddNeighbour(forward_node);
}

void bootstrap() {
    std::shared_ptr<Point> root_node = Point::GetInstance(PolarCoord(WORLD_RADIUS, CIRCLE_DIVISIONS /4, 0));
    std::shared_ptr<Point> bottom_node = Point::GetInstance(PolarCoord(WORLD_RADIUS, -CIRCLE_DIVISIONS /4, 0));

    std::shared_ptr<Point> new_point_high(nullptr), new_point_low(nullptr), first_point_high(nullptr), first_point_low(nullptr),
            last_point_high(nullptr), last_point_low(nullptr);

    for(int p = 0; p < CIRCLE_DIVISIONS; p += CIRCLE_DIVISIONS / 5){
        new_point_high = Point::GetInstance(PolarCoord(WORLD_RADIUS, CIRCLE_DIVISIONS / 10, p));
        root_node->AddNeighbour(new_point_high);
        if(last_point_high){
            last_point_high->AddNeighbour(new_point_high);
        }

        new_point_low = Point::GetInstance(PolarCoord(WORLD_RADIUS, -CIRCLE_DIVISIONS / 10, p - (CIRCLE_DIVISIONS / 10)));
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

