#ifndef _EDAPP_H_
#define _EDAPP_H_

#include <vector>

#include "matrix4.h"
#include "vec3.h"
#include "EDCamera.h"
#include "EDFrame.h"
#include "Timer.h"
#include "AnimationBlender.h"
#include "Animation.h"
#include "Mesh.h"
#include "Entity.h"
#include "Deformer.h"

class EDApp
{
    // Private constructor for singleton architecture
    EDApp( );

    // Disable copy constructor and assignment operator
    EDApp( const EDApp& e );
    EDApp& operator=( const EDApp& e );

public:

    ~EDApp( );

    static EDApp& GetInstance( );

    void Initialize( );
    void Update( );
    void Shutdown( );

    EDCamera& GetCamera( );

private:

    void DrawGround( );
    void DrawAxes( float scale = 1.0f );

    EDCamera camera;
    EDFrame camera_frame;

    Timer timer;

    Animation< matrix4f > idle_animation;
    Animation< matrix4f > walk_animation;
    Animation< matrix4f > punch_animation;
    std::vector< Mesh > meshes;
    Deformer deformer;
    
    Entity entity;
};

#endif // _EDAPP_H_