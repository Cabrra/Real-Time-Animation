////////////////////////////////////////////////////////////////////////////////
// EDApp.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _EDAPP_H_
#define _EDAPP_H_

#include "Glee.h"
#include <gl/glu.h>

#include "AppConfig.h"
#include "EDCamera.h"
#include "Timer.h"
#include "Mesh.h"
#include "Animation.h"
#include "ProcInterpolator.h"
#include "Deformer.h"

class EDApp
{
    EDApp( ){ }
    ~EDApp( ){ }

    EDCamera m_Camera;
    EDFrame m_CameraFrame;

	// Our skin
    std::vector< Mesh > rigid_meshes;
	std::vector< Mesh > smooth_meshes;

	// Some animation that we will pretend is being applied
	Animation< matrix4f > rigid_animation;
    Animation< matrix4f > smooth_animation;

	// Interpolator that is generating our procedural animation for this example,
	// but would usually be doing practically the same thing as an articulated animation interpolator
	ProcInterpolator rigid_interpolator;
    ProcInterpolator smooth_interpolator;

    Deformer rigid_deformer;
    Deformer smooth_deformer;

public: // members

	// Frame timer
	Timer m_Timer;

public:

    //////////////////////////////////////////////////////////////////////////
    // This function gets the instance of the singleton.
    // 
    // Returns
	// EDApp& the instance of the singleton.
    //////////////////////////////////////////////////////////////////////////
    static EDApp& GetInstance( )
    {
        static EDApp instance;
        return instance;
    }

    //////////////////////////////////////////////////////////////////////////
    // This function initalizes opengl and the demo app
    //////////////////////////////////////////////////////////////////////////
    void Initialize( );

	//////////////////////////////////////////////////////////////////////////
	// Shuts the App down
	//////////////////////////////////////////////////////////////////////////
	void Shutdown( );

    //////////////////////////////////////////////////////////////////////////
    // This is to be called when a 
    // repaint is needed.
    //////////////////////////////////////////////////////////////////////////
    void Update( );

    EDCamera& GetCamera( ) { return m_Camera; }
};

#endif // _EDAPP_H_