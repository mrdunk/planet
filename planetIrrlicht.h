#ifndef __PLANET_IRRLICHT__
#define __PLANET_IRRLICHT__

#define UNUSED(x) (void)(x)

#include <iostream>     // std:cerr
#include <irrlicht.h>

#include "data.h"

#define VERTEX_NUMBER 64000

using namespace irr;

// Inherit from irr::scene::ISceneNode so we can use this as a regular scene node. 
class PlanetSceneNode : public scene::ISceneNode {
  private:
    core::aabbox3d<f32> Box;
    video::S3DVertex Vertices[VERTEX_NUMBER];
    video::SMaterial Material;

    u16 Indices[VERTEX_NUMBER *3];
    int vertice_count_;
    int face_count_;

  public:
    // Args:
    //   parent: Parent of the scene node.
    //   mgr: Pointer to the scene manager.
    //   id: Id of this scene node.
    PlanetSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id);

    // Register this node for drawing.
    virtual void OnRegisterSceneNode() {
        if (IsVisible) {
            SceneManager->registerNodeForRendering(this);
        }

        // Make any child scene nodes register themselves too.
        ISceneNode::OnRegisterSceneNode();
    }

    // Render the scene. Called every frame.
    virtual void render();

    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return Box;
    }

    virtual u32 getMaterialCount() const
    {
        return 1;
    }

    virtual video::SMaterial& getMaterial(u32 i)
    {
        UNUSED(i);
        return Material;
    }

    void Populate();
};

#endif  // __PLANET_IRRLICHT__
