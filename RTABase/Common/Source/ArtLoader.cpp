#include "ArtLoader.h"

#include <fstream>

#include "tinyxml.h"
#include "Animation.h"
#include "Mesh.h"
#include "matrix4.h"
using namespace std;

bool ArtLoader::Load(const std::string& mesh_file_name, const std::string& anim_file_name,
	std::vector< Mesh >& meshes, Animation< matrix4f >& animation)
{
	// TODO
	fstream binfin;

	binfin.open(mesh_file_name, ios_base::in | ios_base::binary);

	if (binfin.is_open())
	{
		//Animation<vec3f>::KeyFrame myKeys;
		int numMesh;
		int numVert;
		Mesh myMesh;
		Mesh::UniqueVertex myUniques;

		binfin.read((char*)&numMesh, sizeof(int));

		for (int i = 0; i < numMesh; i++)
		{
			vector<vec3f> positions;
			vector<vec3f> normals;
			binfin.read((char*)&numVert, sizeof(int));
			Mesh myMesh;
			for (int j = 0; j < numVert; j++)
			{
				vec3f tempV;
				binfin.read((char*)&tempV, sizeof(vec3f));
				positions.push_back(tempV);
			}
			for (int j = 0; j < numVert; j++)
			{
				vec3f tempN;
				binfin.read((char*)&tempN, sizeof(vec3f));
				normals.push_back(tempN);
			}

			//save
			std::vector< Mesh::UniqueVertex > &uniqeVector = myMesh.GetVertices();
			for (int j = 0; j < numVert; j++)
			{
				Mesh::UniqueVertex uniques;
				uniques.position = positions[j];
				uniques.normal = normals[j];

				uniqeVector.push_back(uniques);
			}
			meshes.push_back(myMesh);

		}
	}
	binfin.close();
	//animation
	fstream binfin2;
	binfin2.open(anim_file_name, ios_base::in | ios_base::binary);
	if (binfin2.is_open())
	{
		//Animation<vec3f>::KeyFrame myKeys;
		int channels;
		int numKeys;
		Mesh myMesh;
		Mesh::UniqueVertex myUniques;

		vector<vec3f> positions, normals;

		binfin2.read((char*)&channels, sizeof(int));

		for (int i = 0; i < channels; i++)
		{
			binfin2.read((char*)&numKeys, sizeof(int));

			Animation<matrix4f>::Channel myChannels;
			for (int j = 0; j < numKeys; j++)
			{
				float key_time;
				matrix4f transformer;
				Animation<matrix4f>::Key myKey;

				binfin2.read((char*)&key_time, sizeof(float));
				binfin2.read((char*)&transformer, sizeof(matrix4f));

				myKey.node = transformer;
				myKey.time = key_time;
				myChannels.keys.push_back(myKey);

			}
			animation.channels.push_back(myChannels);
		}
		binfin2.close();
		return true;
	}

	return false;
}