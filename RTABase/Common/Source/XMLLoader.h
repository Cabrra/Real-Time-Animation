#ifndef _XML_LOADER_H_
#define _XML_LOADER_H_

#include <string>
#include <vector>

template < typename T >
class Animation;
class Mesh;

template < typename T >
struct matrix4;
typedef matrix4<float> matrix4f;

class XMLLoader
{
public:

    static bool LoadMesh( const std::string& file_name, std::vector< Mesh >& meshes );
    static bool LoadAnimation( const std::string& file_name, Animation< matrix4f >& animation );
};

#endif // _XML_LOADER_H_