#ifndef _ART_LOADER_H_
#define _ART_LOADER_H_

#include <string>
#include <vector>

template < typename T >
class Animation;
class Mesh;

template < typename T >
struct matrix4;
typedef matrix4<float> matrix4f;

class ArtLoader
{
public:

    static bool Load( const std::string& mesh_file_name, const std::string& anim_file_name,
        std::vector< Mesh >& meshes, Animation< matrix4f >& animation );
};

#endif // _ART_LOADER_H_