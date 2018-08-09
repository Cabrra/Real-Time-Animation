#include "Mesh.h"

Mesh::Mesh( )
{
}

Mesh::~Mesh( )
{
}

bool Mesh::LoadTexture( const char* file_name )
{
    OGLTexture texture;
    std::string s;
    bool result;

    s = file_name;
    s = "..\\Assets\\" + s;

    result = texture.Load( s.c_str( ));
    textures.push_back( texture );

    return result;
}

std::string& Mesh::GetName( )
{
    return name;
}

std::vector< Mesh::UniqueVertex >& Mesh::GetVertices( )
{
    return vertices;
}

std::vector< unsigned int >& Mesh::GetIndices( )
{
    return indices;
}

std::vector< Mesh::Joint >& Mesh::GetJoints( )
{
    return joints;
}

std::vector< std::vector< Mesh::JointInfluence >>& Mesh::GetInfluences( )
{
    return influences;
}

std::vector< std::string >& Mesh::GetTextureNames( )
{
    return texture_names;
}