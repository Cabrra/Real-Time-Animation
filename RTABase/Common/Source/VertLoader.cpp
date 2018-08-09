#include "VertLoader.h"

#include <fstream>

#include "Animation.h"
#include "vec3.h"
using namespace std;

bool VertLoader::Load( const std::string& file_name, Animation< vec3f >& animation )
{
    // TODO

	fstream binfin;

	binfin.open(file_name, ios_base::in | ios_base::binary);

	if (binfin.is_open()) 
	{
		Animation<vec3f>::KeyFrame myKeys;
		int numKeyFrame;
		float time;
		int numVert;


		binfin.read((char*)&numKeyFrame, sizeof(int));

		for (int i = 0; i < numKeyFrame; i++)
		{
			binfin.read((char*)&time, sizeof(float));
			binfin.read((char*)&numVert, sizeof(int));

			myKeys.time = time;
			for (int j = 0; j < numVert; j++)
			{
				vec3f temp;
				binfin.read((char*)&temp, sizeof(vec3f));
				myKeys.nodes.push_back(temp);
			}
			animation.key_frames.push_back(myKeys);
			myKeys.nodes.clear();
		}
		binfin.close();
		return true;
	}

    return false;
}