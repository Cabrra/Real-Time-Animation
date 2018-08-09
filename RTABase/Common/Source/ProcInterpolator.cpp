#include "ProcInterpolator.h"

ProcInterpolator::ProcInterpolator( )
{
    angle_total = 0.0f;
    angle = 0.0005f;
}

ProcInterpolator::~ProcInterpolator( )
{

}

void ProcInterpolator::Process( )
{
    pose.resize( 2 );

    // Some simple procedural animation...
	angle_total += angle;

	if ( angle_total >= 1.0f || angle_total <= -1.0f )
		angle = -angle;

    pose[0] = animation->channels[0].keys[0].node;
    pose[0].make_rotation_z( angle_total );

    pose[1] = pose[0];
    pose[1].translate_pre( 4.0f, 0.0f, 0.0f );
    pose[1].rotate_z_pre( -angle_total*2.0f );
}