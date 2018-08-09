#ifndef _DEFORMER_H_
#define _DEFORMER_H_

#include <vector>

template < typename T >
struct matrix4;
typedef matrix4<float> matrix4f;

template < typename T >
struct vec3;
typedef vec3<float> float3;

class Mesh;

class Deformer
{
public:

    Deformer( );
    ~Deformer( );

    void Process( const std::vector< matrix4f >& pose );

    void SetMesh( Mesh* mesh );
    Mesh* GetMesh( ) const;

    std::vector< float3 >& GetSkinVerts( );

private:

    void OrthoNormalInverse( matrix4f& r, const matrix4f& m );
    void Multiply( float3& r, const float3& v, const matrix4f& m );

    Mesh* mesh;
    std::vector< float3 > skin_verts;
};

#endif // _DEFORMER_H_