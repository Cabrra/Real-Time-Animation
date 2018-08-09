#include "Deformer.h"

#include "matrix4.h"
#include "vec3.h"
#include "Mesh.h"

Deformer::Deformer( )
{
    mesh = 0;
}

Deformer::~Deformer( )
{
}

void Deformer::Process( const std::vector< matrix4f >& pose )
{
    if ( mesh == 0 || mesh->GetJoints( ).size( ) == 0 )
    {
        return;
    }

    skin_verts.resize( mesh->GetVertices( ).size( ));

    // TODO: Fill out "skin_verts"
    //   "mesh->GetJoints( )" contains the joints in their bind pose
    //   "pose" contains the interpolated joints
    //   "mesh->GetVertices( )" contains the skin space vertices of the model
    //   "mesh->GetInfluences( )" contains a vector of influence information for each unique vertex

	size_t mySize = mesh->GetVertices().size();

	for (size_t i = 0; i < mySize; i++)
	{
		size_t influ = mesh->GetInfluences()[i].size();
		vec3f vSum =  vec3f(0.0f, 0.0f, 0.0f);

		for (size_t j = 0; j < influ; j++)
		{
			matrix4f inversal;
			inversal.make_identity();
			OrthoNormalInverse(inversal, mesh->GetJoints()[mesh->GetInfluences()[i][j].joint_index].world_bind_pose_transform);
			vec3f semiresult = vec3f(0.0f, 0.0f, 0.0f);;
			Multiply(semiresult, mesh->GetVertices()[i].position, inversal);
			vec3f result = vec3f(0.0f, 0.0f, 0.0f);;
			Multiply(result, semiresult, pose[mesh->GetInfluences()[i][j].joint_index]);

			vSum.x += mesh->GetInfluences()[i][j].weight * result.x;
			vSum.y += mesh->GetInfluences()[i][j].weight * result.y;
			vSum.z += mesh->GetInfluences()[i][j].weight * result.z;
		
		}

		skin_verts[i] = vSum;
	}
}

void Deformer::SetMesh( Mesh* mesh )
{
    this->mesh = mesh;
}

Mesh* Deformer::GetMesh( ) const
{
    return mesh;
}

std::vector< float3 >& Deformer::GetSkinVerts( )
{
    return skin_verts;
}

void Deformer::OrthoNormalInverse( matrix4f& r, const matrix4f& m )
{
    // TODO: Take the orthonormal inverse of m and store it in r
	r.make_identity();
	r.axis_x.x = m.axis_x.x;
	r.axis_x.y = m.axis_y.x;
	r.axis_x.z = m.axis_z.x;

	r.axis_y.x = m.axis_x.y;
	r.axis_y.y = m.axis_y.y;
	r.axis_y.z = m.axis_z.y;

	r.axis_z.x = m.axis_x.z;
	r.axis_z.y = m.axis_y.z;
	r.axis_z.z = m.axis_z.z;

	r.axis_pos = m.axis_pos;
	/*r = m;
	r.invert()*/

	r.axis_w.x = -dot_product(m.axis_x, m.axis_w);
	r.axis_w.y = -dot_product(m.axis_y, m.axis_w);
	r.axis_w.z = -dot_product(m.axis_z, m.axis_w);
}

void Deformer::Multiply( float3& r, const float3& v, const matrix4f& m )
{
    // TODO: Multiply m and v, store the result in r
	r.x = m.axis_x.x * v.x + m.axis_y.x * v.y + m.axis_z.x * v.z + m.axis_w.x;
	r.y = m.axis_x.y * v.x + m.axis_y.y * v.y + m.axis_z.y * v.z + m.axis_w.y;
	r.z = m.axis_x.z * v.x + m.axis_y.z * v.y + m.axis_z.z * v.z + m.axis_w.z;
}