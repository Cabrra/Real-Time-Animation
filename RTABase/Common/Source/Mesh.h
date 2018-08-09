#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <vector>
#include "matrix4.h"
#include "vec3.h"
#include "OGLTexture.h"

class Mesh
{
public:

    struct TexCoord
    {
        float u, v;
    };

    struct Joint
    {
        // The name of the joint
        std::string name;

        // The joints parent
        unsigned int parent;

        // The joints children
        std::vector< unsigned int > children;

        // Bind pose transform in local space
        matrix4f local_bind_pose_transform;

        // Bind pose transform in world space
        matrix4f world_bind_pose_transform;
    };

    struct JointInfluence
    {
        unsigned int joint_index;
        float weight;
    };

    struct UniqueVertex
    {
        float3 position;
        float3 normal;
        TexCoord tex_coord;

        int control_point_index;

        bool operator==( const UniqueVertex& v ) const
        {
            return position.x == v.position.x &&
                position.y == v.position.y &&
                position.z == v.position.z &&
                normal.x == v.normal.x &&
                normal.y == v.normal.y &&
                normal.z == v.normal.z &&
                tex_coord.u == v.tex_coord.u &&
                tex_coord.v == v.tex_coord.v &&
                control_point_index == v.control_point_index;
        }
    };

public:

    Mesh( );
    ~Mesh( );

    bool LoadTexture( const char* file_name );

    std::string& GetName( );

    std::vector< UniqueVertex >& GetVertices( );

    std::vector< unsigned int >& GetIndices( );

    std::vector< Joint >& GetJoints( );
    std::vector< std::vector< JointInfluence >>& GetInfluences( );

    std::vector< std::string >& GetTextureNames( );

private:

    std::string name;

    std::vector< UniqueVertex > vertices;

    std::vector< unsigned int > indices;

    std::vector< Joint > joints;
    std::vector< std::vector< JointInfluence >> influences;

    std::vector< std::string > texture_names;
    std::vector< OGLTexture > textures;
};

#endif // _MESH_H_