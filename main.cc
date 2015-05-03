#include <iostream>


#include "data.h"
#include "planetIrrlicht.h"
//#include "driverChoice.h"  // Ask user which graphics system to use.

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{
    // Initialise planet data.
    bootstrap();


    // Driver type. OpenGL, DirectX, SW, etc.
    video::E_DRIVER_TYPE driverType = (video::E_DRIVER_TYPE)5;  // driverChoiceConsole();
    if (driverType==video::EDT_COUNT) {
        return 1;
    }

    // create device
    IrrlichtDevice *device = createDevice(driverType,
            core::dimension2d<u32>(640, 640), 16, false);

    if (device == 0) {
        return 1; // could not create selected driver.
    }

    // create engine and camera
    device->setWindowCaption(L"Hello World.");

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* scene_manager = device->getSceneManager();

    scene_manager->addCameraSceneNode(0, core::vector3df(100, -WORLD_RADIUS *4, 0), core::vector3df(0,0,0));

    PlanetSceneNode *planet_node =
        new PlanetSceneNode(scene_manager->getRootSceneNode(), scene_manager, 666);


    scene::ISceneNodeAnimator* anim =
        scene_manager->createRotationAnimator(core::vector3df(0.25f, 0, 0));

    if(anim) {
        planet_node->addAnimator(anim);

        // I'm done referring to anim, so must
        // irr::IReferenceCounted::drop() this reference now because it
        // was produced by a createFoo() function. As I shouldn't refer to
        // it again, ensure that I can't by setting to 0.
        anim->drop();
        anim = 0;
    }

    // Done reffering to planet_node directly so drop reference to it.
    planet_node->drop();
    planet_node = 0;


    // Now draw the things.
    u32 frames=0;
    while(device->run()) {
        driver->beginScene(true, true, video::SColor(0,100,100,100));

        scene_manager->drawAll();

        driver->endScene();
        if (++frames==100) {
            core::stringw str = L"Irrlicht Engine [";
            str += driver->getName();
            str += L"] FPS: ";
            str += (s32)driver->getFPS();

            device->setWindowCaption(str.c_str());
            frames=0;
        }
    }

    device->drop();

    return 0;


    //std::cout << root_node.use_count() << '\n';
} 
