#ifndef _EDAPP_H_
#define _EDAPP_H_

#include <vector>

#include "matrix4.h"
#include "vec3.h"
#include "EDCamera.h"
#include "EDFrame.h"
#include "Timer.h"
#include "Mesh.h"
#include "Animation.h"
#include "Interpolator.h"
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

    std::vector< Mesh > meshes;
    Animation< matrix4f > animation;
    Interpolator< matrix4f > interpolator;
    Deformer deformer;
};

#endif // _EDAPP_H_