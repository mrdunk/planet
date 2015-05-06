
#include "planetIrrlicht.h"

PlanetSceneNode::PlanetSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
        : scene::ISceneNode(parent, mgr, id) {
    Material.Wireframe = false;
    //Material.Wireframe = true;
    Material.Lighting = false;
    //Material.BackfaceCulling = false;

    Populate();

    std::cout << "vertice_count_: " << vertice_count_ << "\tface_count_: " << face_count_ << std::endl;

    /*
       The Irrlicht Engine needs to know the bounding box of a scene node.
       It will use it for automatic culling and other things. Hence, we
       need to create a bounding box from the 4 vertices we use.
       If you do not want the engine to use the box for automatic culling,
       and/or don't want to create the box, you could also call
       irr::scene::ISceneNode::setAutomaticCulling() with irr::scene::EAC_OFF.
     */
    Box.reset(core::vector3d<float>(-WORLD_RADIUS, -WORLD_RADIUS, -WORLD_RADIUS));
    Box.addInternalPoint(core::vector3d<float>(WORLD_RADIUS, -WORLD_RADIUS, -WORLD_RADIUS));
    Box.addInternalPoint(core::vector3d<float>(WORLD_RADIUS, WORLD_RADIUS, -WORLD_RADIUS));
    Box.addInternalPoint(core::vector3d<float>(WORLD_RADIUS, WORLD_RADIUS, WORLD_RADIUS));
}

void PlanetSceneNode::Populate(){
    vertice_count_ = 0;
    face_count_ = 0;

    auto it_face = FaceIterator(0);
    Face face = it_face.begin();
    while(face != it_face.end()){
        //std::cout << vertice_count_ <<"\t" << face_count_ << std::endl;

        std::shared_ptr<Point> p0 = std::get<0>(face);
        std::shared_ptr<Point> p1 = std::get<1>(face);
        std::shared_ptr<Point> p2 = std::get<2>(face);

        CartesianCoord c0 = ToCartesian(PolarCoord(p0->radius_, p0->lattitude_, p0->longditude_));
        CartesianCoord c1 = ToCartesian(PolarCoord(p1->radius_, p1->lattitude_, p1->longditude_));
        CartesianCoord c2 = ToCartesian(PolarCoord(p2->radius_, p2->lattitude_, p2->longditude_));

        /*DisplayCoord(c0);
        DisplayCoord(c1);
        DisplayCoord(c2);
        std::cout << std::endl;*/

        Vertices[vertice_count_ +0] = video::S3DVertex(C_Z(c0), C_Y(c0), C_X(c0), 1,1,0, video::SColor(255,0,0,255), 0, 1);
        Vertices[vertice_count_ +1] = video::S3DVertex(C_Z(c1), C_Y(c1), C_X(c1), 1,1,0, video::SColor(255,vertice_count_ *10,0,255), 0, 1);
        Vertices[vertice_count_ +2] = video::S3DVertex(C_Z(c2), C_Y(c2), C_X(c2), 1,1,0, video::SColor(255,0,vertice_count_ *10,255), 0, 1);

        Indices[(face_count_ *3) +0] = vertice_count_ +0;
        Indices[(face_count_ *3) +1] = vertice_count_ +1;
        Indices[(face_count_ *3) +2] = vertice_count_ +2;
        
        vertice_count_ += 3;
        face_count_ ++;
        face = it_face.GetFace();

        if(vertice_count_ > VERTEX_NUMBER) {
            std::cerr << "Exceded VERTEX_NUMBER.\n" << std::endl;
        }
    }
}

void PlanetSceneNode::Populate2(){
    vertice_count_ = 0;
    face_count_ = 0;
    for(auto it_point = Point::all_points_.begin(); it_point!=Point::all_points_.end(); ++it_point){
        DisplayPolarCoord(it_point->first);
        CartesianCoord coordinate = ToCartesian(it_point->first);
        DisplayCoord(coordinate);

        Vertices[vertice_count_] = video::S3DVertex(C_Z(coordinate), C_Y(coordinate), C_X(coordinate), 1,1,0, video::SColor(255,0,0,255), 0, 1);
        int centre_vertice_count = vertice_count_;
        ++vertice_count_;

        std::shared_ptr<Point> last_point(nullptr), first_point(nullptr);
        if(it_point->second->neighbours_count_ > 1){
            for(int neighbour_index = 0; neighbour_index < it_point->second->neighbours_count_; ++neighbour_index){
                coordinate = ToCartesian(PolarCoord(it_point->second->neighbours_[neighbour_index]->radius_, 
                            it_point->second->neighbours_[neighbour_index]->lattitude_, 
                            it_point->second->neighbours_[neighbour_index]->longditude_));
                Vertices[vertice_count_] = video::S3DVertex(C_Z(coordinate), C_Y(coordinate), C_X(coordinate), 1,1,0, video::SColor(255,vertice_count_ *2,255,255), 0, 1);
                if(last_point && last_point->IsNeighbour(it_point->second->neighbours_[neighbour_index])){
                    Indices[(face_count_ *3)] = centre_vertice_count;
                    Indices[(face_count_ *3) +1] = vertice_count_ -1;
                    Indices[(face_count_ *3) +2] = vertice_count_;
                    ++face_count_;
                }
                if(!first_point){
                    first_point = it_point->second->neighbours_[neighbour_index];
                }
                last_point = it_point->second->neighbours_[neighbour_index];
                ++vertice_count_;
            }
            if(last_point && first_point && last_point->IsNeighbour(first_point)){
                Indices[(face_count_ *3)] = centre_vertice_count;
                Indices[(face_count_ *3) +2] = vertice_count_ -1;
                Indices[(face_count_ *3) +1] = centre_vertice_count +1;
                ++face_count_;
            }
        }

        if(vertice_count_ > VERTEX_NUMBER) {
            std::cerr << "Exceded VERTEX_NUMBER.\n" << std::endl;
        }
        //break;
    }
}

void PlanetSceneNode::render() {
    video::IVideoDriver* driver = SceneManager->getVideoDriver();

    driver->setMaterial(Material);
    driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
    driver->drawVertexPrimitiveList(&Vertices[0], vertice_count_, &Indices[0], face_count_, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
}
