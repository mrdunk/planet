
#include "planetIrrlicht.h"

PlanetSceneNode::PlanetSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
        : scene::ISceneNode(parent, mgr, id) {
    //Material.Wireframe = false;
    Material.Wireframe = true;
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

    //setAutomaticCulling(irr::scene::EAC_OFF);
    //setDebugDataVisible(irr::scene::EDS_BBOX);
}

void PlanetSceneNode::Populate(){
    vertice_count_ = 0;
    face_count_ = 0;

    auto it_face = FaceIterator(1);
    Face face = it_face.begin();

    while(face != it_face.end()){
        CartesianCoord c0 = face.p0_->GetCartesianCoord();
        CartesianCoord c1 = face.p1_->GetCartesianCoord();
        CartesianCoord c2 = face.p2_->GetCartesianCoord();

        DisplayCoord(c0);
        DisplayCoord(c1);
        DisplayCoord(c2);
        std::cout << std::endl;

        if(abs(C_Z(c0)) > WORLD_RADIUS/2 || abs(C_Z(c1)) > WORLD_RADIUS/2 || abs(C_Z(c2)) > WORLD_RADIUS/2){
            Vertices[vertice_count_ +0] = video::S3DVertex(C_Z(c0), C_Y(c0), C_X(c0), 1,1,0, video::SColor(255,255,255,255), 0, 1);
            Vertices[vertice_count_ +1] = video::S3DVertex(C_Z(c1), C_Y(c1), C_X(c1), 1,1,0, video::SColor(255,255,255,255), 0, 1);
            Vertices[vertice_count_ +2] = video::S3DVertex(C_Z(c2), C_Y(c2), C_X(c2), 1,1,0, video::SColor(255,255,255,255), 0, 1);
        } else {
            Vertices[vertice_count_ +0] = video::S3DVertex(C_Z(c0), C_Y(c0), C_X(c0), 1,1,0, video::SColor(255,0,0,255), 0, 1);
            Vertices[vertice_count_ +1] = video::S3DVertex(C_Z(c1), C_Y(c1), C_X(c1), 1,1,0, video::SColor(255,vertice_count_ *10,0,255), 0, 1);
            Vertices[vertice_count_ +2] = video::S3DVertex(C_Z(c2), C_Y(c2), C_X(c2), 1,1,0, video::SColor(255,0,vertice_count_ *10,255), 0, 1);
        }

        Indices[(face_count_ *3) +0] = vertice_count_ +0;
        Indices[(face_count_ *3) +1] = vertice_count_ +1;
        Indices[(face_count_ *3) +2] = vertice_count_ +2;
        
        vertice_count_ += 3;
        face_count_ ++;
        face = it_face.GetFace();

        if(vertice_count_ > VERTEX_NUMBER) {
            std::cerr << "Exceded VERTEX_NUMBER." << std::endl;
            std::cerr << "vertice_count_: " << vertice_count_ << std::endl << std::endl;
        }
    }
}

void PlanetSceneNode::render() {
    video::IVideoDriver* driver = SceneManager->getVideoDriver();

    driver->setMaterial(Material);
    driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
    driver->drawVertexPrimitiveList(&Vertices[0], vertice_count_, &Indices[0], face_count_, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
}
