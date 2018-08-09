#include "TubeLoader.h"

#include <fstream>

#include "Animation.h"
#include "Mesh.h"
#include "matrix4.h"

bool TubeLoader::Load( std::vector< Mesh >& meshes, Animation< matrix4f >& animation, bool rigid )
{
	// We won't actually be loading anything for this example,
	// so we'll just procedurally generate a cylinder mesh to use

	const unsigned int uiSegmentCount = 5;
	const float fSegmentLength = 2.0f;
	const unsigned int uiRes = 10;
	const float fRadius = 1.0f;

	// Just the one mesh
	meshes.resize(1);

	meshes[0].GetInfluences( ).resize(uiRes*uiSegmentCount);

	#define EDPI 3.1415926535897932384626433832795f

	for( unsigned int j = 0; j < uiSegmentCount; ++j )
	{
		for( float i = 0.0f; i < 2.0f*EDPI; i += (2.0f*EDPI) / uiRes )
        {
            Mesh::UniqueVertex uv;
            uv.position = vec3f( (float)j*fSegmentLength, cos(i) * fRadius, sin(i) * fRadius );
			meshes[0].GetVertices().push_back( uv );
        }

		unsigned int uiLTIndex = j * uiRes;

		if( j < uiSegmentCount - 1 )
			for( unsigned int i = 0; i < uiRes; ++i )
			{
				meshes[0].GetIndices().push_back( uiLTIndex + i );
				i + 1 == uiRes ? meshes[0].GetIndices().push_back( uiLTIndex ) : meshes[0].GetIndices().push_back( uiLTIndex + i + 1 );
				meshes[0].GetIndices().push_back( uiLTIndex + i + uiRes );

				meshes[0].GetIndices().push_back( uiLTIndex + i + uiRes );
				i + 1 == uiRes ? meshes[0].GetIndices().push_back( uiLTIndex ) : meshes[0].GetIndices().push_back( uiLTIndex + i + 1 );
				i + 1 == uiRes ? meshes[0].GetIndices().push_back( uiLTIndex + uiRes ) : meshes[0].GetIndices().push_back( uiLTIndex + i + 1 + uiRes );
			}

        Mesh::JointInfluence ji;

        if ( rigid == false )
        {
		    for( unsigned int i = 0; i < uiRes; ++i )
		    {
			    float fWeight = (float)j / (float)(uiSegmentCount-1);
			    float fInvWeight = 1.0f - fWeight;

                ji.joint_index = 0;
                ji.weight = fInvWeight;
			    meshes[0].GetInfluences( )[j*uiRes+i].push_back( ji );

                ji.joint_index = 1;
                ji.weight = fWeight;
			    meshes[0].GetInfluences( )[j*uiRes+i].push_back( ji );
		    }
        }
        else
        {
            if( j < 3 )
	            for( unsigned int i = 0; i < uiRes; ++i )
                {
                    ji.joint_index = 0;
                    ji.weight = 1.0f;
		            meshes[0].GetInfluences( )[j*uiRes+i].push_back( ji );
                }
            else
	            for( unsigned int i = 0; i < uiRes; ++i )
                {
		            ji.joint_index = 1;
                    ji.weight = 1.0f;
		            meshes[0].GetInfluences( )[j*uiRes+i].push_back( ji );
                }
        }
	}


    // We'll be doing a prodedural animation in the interpolator for this example, so the only "animation" data
	// we will have will be our bind pose

	animation.channels.resize(2);

    animation.channels[0].keys.resize( 1 );
    animation.channels[0].keys[0].time = 0.0f;

    animation.channels[1].keys.resize( 1 );
    animation.channels[1].keys[0].time = 0.0f;

    animation.channels[0].keys[0].node.make_identity( );
    animation.channels[1].keys[0].node.make_identity( );
    animation.channels[1].keys[0].node.axis_pos.x = 4.0f;

    // Set the bind pose in the mesh as well
    meshes[0].GetJoints( ).resize( 2 );

    meshes[0].GetJoints( )[0].world_bind_pose_transform = animation.channels[0].keys[0].node;
    meshes[0].GetJoints( )[1].world_bind_pose_transform = animation.channels[1].keys[0].node;

    return true;
}