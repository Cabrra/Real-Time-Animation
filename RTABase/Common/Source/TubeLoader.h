#ifndef _TUBE_LOADER_H_
#define _TUBE_LOADER_H_

#include <string>
#include <vector>

template < typename T >
class Animation;
class Mesh;

template < typename T >
struct matrix4;
typedef matrix4<float> matrix4f;

class TubeLoader
{
public:

    static bool Load( std::vector< Mesh >& meshes, Animation< matrix4f >& animation, bool rigid );
};

#endif // _TUBE_LOADER_H_