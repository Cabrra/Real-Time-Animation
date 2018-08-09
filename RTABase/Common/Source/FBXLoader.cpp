#include "FBXLoader.h"

#include <algorithm>
#include <fbxsdk/fileio/fbxiosettings.h>

#include "Mesh.h"
#include "Animation.h"

void TransformMatrix(FbxAMatrix thisguy, matrix4f& tothisguy)
{

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			tothisguy.m[x][y] = (float)thisguy.Get(x, y);
		}
	}
}

bool FBXLoader::Load( const std::string& file_name, std::vector< Mesh >& meshes, Animation< matrix4f >& animation )
{
    // Get an FBX manager
    FbxManager* manager = FbxManager::Create( );
    if ( manager == 0 )
    {
        return false;
    }

    // Create IO settings
    FbxIOSettings* io_settings = FbxIOSettings::Create( manager, IOSROOT );
    if ( io_settings == 0 )
    {
        return false;
    }

    manager->SetIOSettings( io_settings );

    // Create importer
    FbxImporter* importer = FbxImporter::Create( manager, "" );
    if ( importer == 0 )
    {
        return false;
    }

    // Initialize importer
    if ( importer->Initialize( file_name.c_str( ), -1, io_settings ) == false )
    {
        std::string error;
        error += "FbxImporter::Initialize failed:  ";
        error += importer->GetStatus( ).GetErrorString( );
        MessageBox( NULL, error.c_str( ), "Error", MB_OK );
        return false;
    }

    // Create a scene
    FbxScene* scene = FbxScene::Create( manager, "myScene" );
    if ( scene == 0 )
    {
        return false;
    }

    // Load the scene with contents from the file
    if ( importer->Import( scene ) == false )
    {
        return false;
    }

    // No longer need the importer
    importer->Destroy( );

    // Our list of unique joints
    JointList unique_joints;

    // Traverse the scene
    FbxNode* root_node = scene->GetRootNode( );
    if ( ProcessNode( root_node, meshes, unique_joints ) == false )
    {
        return false;
    }

    // Get the number of animation stacks
    int num_anim_stacks = scene->GetSrcObjectCount< FbxAnimStack >( );

    FbxAnimStack* anim_stack;
    for ( int i = 0; i < num_anim_stacks; ++i )
    {
        // Get the current animation stack
        anim_stack = scene->GetSrcObject< FbxAnimStack >( i );

        FbxString animation_name = anim_stack->GetName( );
        animation.name = animation_name;

        // Get the number of animation layers in the current animation stack
        int num_anim_layers = anim_stack->GetMemberCount< FbxAnimLayer >( );

        FbxAnimLayer* anim_layer;
        for ( int j = 0; j < num_anim_layers; ++j )
        {
            anim_layer = anim_stack->GetMember< FbxAnimLayer >( j );

            LoadAnimation( anim_layer, animation, unique_joints );
        }
    }

    return true;
}

bool FBXLoader::ProcessNode( FbxNode* node, std::vector< Mesh >& meshes, JointList& unique_joints )
{
    // TODO

	if (node->GetMesh() != nullptr)
	{
		fbxsdk_2015_1::FbxMesh* myMesh = node->GetMesh();
		Mesh newMesh;

		LoadMesh(myMesh, newMesh);
		LoadSkin(myMesh, newMesh, unique_joints);
		LoadTexture(myMesh, newMesh);

		meshes.push_back(newMesh);
		/*std::pair <fbxsdk_2015_1::FbxNode*, fbxsdk_2015_1::FbxNodeAttribute*> myPair;
		myPair.first = node;
		myPair.second = node->GetNodeAttribute();
		unique_joints.push_back(myPair);*/
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), meshes, unique_joints);

	return true;
    
}

bool FBXLoader::LoadMesh( FbxMesh* fbx_mesh, Mesh& mesh )
{
    int polygon_count;
    int polygon;
    int polygon_vertex_count;
    int polygon_vertex;
    int uv_element_count;
    int uv_element;
    int normal_element_count;
    int normal_element;
    int vertex_id;

    polygon_count = fbx_mesh->GetPolygonCount( );
    FbxVector4* control_points = fbx_mesh->GetControlPoints( );

    int control_point_count = fbx_mesh->GetControlPointsCount( );

    vertex_id = 0;

    for ( polygon = 0; polygon < polygon_count; ++polygon )
    {
        polygon_vertex_count = fbx_mesh->GetPolygonSize( polygon );
		for ( polygon_vertex = 0; polygon_vertex < polygon_vertex_count; ++polygon_vertex )
		{
            Mesh::UniqueVertex unique_vertex;

			int control_point_index = fbx_mesh->GetPolygonVertex( polygon, polygon_vertex );

            unique_vertex.control_point_index = control_point_index;
            unique_vertex.position.x = ( float )control_points[control_point_index].mData[0];
            unique_vertex.position.y = ( float )control_points[control_point_index].mData[1];
            unique_vertex.position.z = ( float )control_points[control_point_index].mData[2];

            // Get texture coords
            uv_element_count = fbx_mesh->GetElementUVCount( );
			for ( uv_element = 0; uv_element < uv_element_count; ++uv_element )
			{
				FbxGeometryElementUV* geometry_element_uv = fbx_mesh->GetElementUV( uv_element );

                FbxLayerElement::EMappingMode mapping_mode = geometry_element_uv->GetMappingMode( );
                FbxLayerElement::EReferenceMode reference_mode = geometry_element_uv->GetReferenceMode( );

                int direct_index = -1;

                // eByControlPoint:  One control point has one uv
                // eByPolygonVertex:  One control point can have multiple uv's
                if ( mapping_mode == FbxGeometryElement::eByControlPoint )
                {
                    if ( reference_mode == FbxGeometryElement::eDirect )
                    {
                        direct_index = control_point_index;
                    }
                    else if ( reference_mode == FbxGeometryElement::eIndexToDirect )
                    {
                        direct_index = geometry_element_uv->GetIndexArray( ).GetAt( control_point_index );
                    }
                }
                else if ( mapping_mode == FbxGeometryElement::eByPolygonVertex )
                {
                    if ( reference_mode == FbxGeometryElement::eDirect ||
                        reference_mode == FbxGeometryElement::eIndexToDirect )
                    {
                        direct_index = fbx_mesh->GetTextureUVIndex( polygon, polygon_vertex );
                    }
                }

                if ( direct_index != -1 )
                {
                    FbxVector2 uv = geometry_element_uv->GetDirectArray( ).GetAt( direct_index );

                    unique_vertex.tex_coord.u = ( float )uv.mData[0];
                    unique_vertex.tex_coord.v = ( float )uv.mData[1];
                }
            }

            // Get normals
            normal_element_count = fbx_mesh->GetElementNormalCount( );
			for ( normal_element = 0; normal_element < normal_element_count; ++normal_element )
			{
				FbxGeometryElementNormal* geometry_element_normal = fbx_mesh->GetElementNormal( normal_element );

                FbxLayerElement::EMappingMode mapping_mode = geometry_element_normal->GetMappingMode( );
                FbxLayerElement::EReferenceMode reference_mode = geometry_element_normal->GetReferenceMode( );

                int direct_index = -1;

				if ( mapping_mode == FbxGeometryElement::eByPolygonVertex )
				{
                    if ( reference_mode == FbxGeometryElement::eDirect )
                    {
                        direct_index = vertex_id;
                    }
                    else if ( reference_mode == FbxGeometryElement::eIndexToDirect )
                    {
                        direct_index = geometry_element_normal->GetIndexArray( ).GetAt( vertex_id );
                    }
				}

                if ( direct_index != -1 )
                {
                    FbxVector4 norm = geometry_element_normal->GetDirectArray( ).GetAt( direct_index );

                    unique_vertex.normal.x = ( float )norm.mData[0];
                    unique_vertex.normal.y = ( float )norm.mData[1];
                    unique_vertex.normal.z = ( float )norm.mData[2];
                }
			}

            std::vector< Mesh::UniqueVertex >& unique_vertices = mesh.GetVertices( );

            size_t size = unique_vertices.size( );
            size_t i;
            for ( i = 0; i < size; ++i )
            {
                if ( unique_vertex == unique_vertices[i] )
                {
                    break;
                }
            }

            if ( i == size )
            {
                unique_vertices.push_back( unique_vertex );
            }

            mesh.GetIndices( ).push_back( i );

			++vertex_id;
		}
    }

    return true;
}

bool FBXLoader::LoadSkin( FbxMesh* fbx_mesh, Mesh& mesh, JointList& unique_joints )
{
    // TODO
	std::vector<Mesh::UniqueVertex> &myVec = mesh.GetVertices();
	std::vector< std::vector< Mesh::JointInfluence >> &myInfluVec = mesh.GetInfluences();

	myInfluVec.resize(myVec.size());

	int count = fbx_mesh->GetDeformerCount();
	for (int i = 0; i < count; i++)
	{
		fbxsdk_2015_1::FbxDeformer* myDeform = fbx_mesh->GetDeformer(i, FbxDeformer::eSkin);
		fbxsdk_2015_1::FbxSkin* mySkin = (fbxsdk_2015_1::FbxSkin*) myDeform;
		int skinCount = mySkin->GetClusterCount();
		for (int j = 0; j < skinCount; j++)
		{
			FbxCluster* myClust = mySkin->GetCluster(j);
			unsigned int myIndex;
			
			LoadSkeletonNode(myClust->GetLink(), mesh, unique_joints, myIndex);

			//bin pose
			matrix4f mytransMa;
			mytransMa.make_identity();
			FbxAMatrix helper1, helper2;

			myClust->GetTransformMatrix(helper1);
			myClust->GetTransformLinkMatrix(helper2);

			helper1.operator*(helper2);

			TransformMatrix(helper2, mytransMa);


			mesh.GetJoints()[myIndex].world_bind_pose_transform = mytransMa;

			int controlCount = myClust->GetControlPointIndicesCount();
			int* pointIndex = myClust->GetControlPointIndices();
			double* pointWeight = myClust->GetControlPointWeights();
				
			for (int z = 0; z < controlCount; z++)
			{
				Mesh::JointInfluence influ;
				influ.joint_index = myIndex;
				influ.weight = (float)pointWeight[z];

				size_t vertexSize = myVec.size();
				for (size_t finalLoop = 0; finalLoop < vertexSize; finalLoop++)
				{
					if (myVec[finalLoop].control_point_index == pointIndex[z])
						myInfluVec[finalLoop].push_back(influ);
				}
			}
			
			
		}

	}
	
    return true;
}

bool FBXLoader::LoadTexture( FbxMesh* fbx_mesh, Mesh& mesh )
{
    FbxProperty property;

    if ( fbx_mesh->GetNode( ) == NULL )
        return false;

    int material_count = fbx_mesh->GetNode( )->GetSrcObjectCount< FbxSurfaceMaterial >( );

    for ( int material_index = 0; material_index < material_count; ++material_index )
    {
        FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode( )->GetSrcObject< FbxSurfaceMaterial >( material_index );

        if ( surface_material == 0 )
            continue;

        int texture_index;

        FBXSDK_FOR_EACH_TEXTURE( texture_index )
        {
            property = surface_material->FindProperty( FbxLayerElement::sTextureChannelNames[ texture_index ]);

            if ( property.IsValid( ) == false )
                continue;

	        int texture_count = property.GetSrcObjectCount< FbxTexture >( );

	        for ( int i = 0; i < texture_count; ++i )
            {
		        // Ignore layered textures

                FbxTexture* texture = property.GetSrcObject< FbxTexture >( i );
                if ( texture == 0 )
                    continue;

                FbxFileTexture* file_texture = FbxCast< FbxFileTexture >( texture );
                if ( file_texture == 0 )
                    continue;

                std::string texture_name = file_texture->GetFileName( );
                std::string::size_type pos = texture_name.find_last_of( '\\' );
                if ( pos != std::string::npos )
                {
                    texture_name = texture_name.substr( pos + 1 );
                }

		        mesh.LoadTexture( texture_name.c_str( ));
            }
        }
    }

    return true;
}

bool FBXLoader::LoadSkeletonNode( FbxNode* node, Mesh& mesh, JointList& unique_joints, unsigned int& index )
{
    // TODO
	if (node->GetSkeleton() != nullptr)
	{
		int myIndex = index;
		bool found = false;
		size_t mySiz = unique_joints.size();
		for (unsigned int  i = 0; i < mySiz; i++)
		{
			if (unique_joints[i].first == node)
			{
				found = true;
				myIndex = i;
				break;
			}
		}
		if (found == false)
		{
			std::pair<fbxsdk_2015_1::FbxNode*, fbxsdk_2015_1::FbxNodeAttribute*> myPair;
			myPair.first = node;
			myPair.second = node->GetNodeAttribute();
			Mesh::Joint myJoint;
			myJoint.name = node->GetName();
			std::vector<Mesh::Joint> &myVec = mesh.GetJoints();
			myVec.push_back(myJoint);
			unique_joints.push_back(myPair);
		
			myIndex = myVec.size() - 1;
			

			int numChild = node->GetChildCount();
			for (int i = 0; i < numChild; i++)
			{
				unsigned int childIndex;
				LoadSkeletonNode(node->GetChild(i), mesh, unique_joints, childIndex);
				myVec[myIndex].children.push_back(childIndex);
			}

			if (node->GetParent() != nullptr)
			{
				unsigned int parent;
				LoadSkeletonNode(node->GetParent(), mesh, unique_joints, parent);
				myVec[myIndex].parent = parent;
			}
			else
				myVec[myIndex].parent = -1;
		}

		index = myIndex;
		return true;
	}
	else
		return false;
}

bool FBXLoader::LoadAnimation( FbxAnimLayer* anim_layer, Animation< matrix4f >& animation, JointList& unique_joints )
{
    // TODO

	unsigned int mySiz = unique_joints.size();
	for (unsigned int i = 0; i < mySiz; i++)
	{
		std::vector< float >myKeyTimes;
		LoadCurve(unique_joints[i].first->LclTranslation.GetCurve(anim_layer), myKeyTimes);
		LoadCurve(unique_joints[i].first->LclRotation.GetCurve(anim_layer), myKeyTimes);
		LoadCurve(unique_joints[i].first->LclScaling.GetCurve(anim_layer), myKeyTimes);

		Animation<matrix4f>::Channel myChannel;
		unsigned int keySiz = myKeyTimes.size();
		for (unsigned int j = 1; j < keySiz; j++)
		{
			Animation<matrix4f>::Key myKey;
			matrix4f myMatrix;
			myMatrix.make_identity();
			FbxTime help;
			help.SetSecondDouble((double)myKeyTimes[j]);
			TransformMatrix(unique_joints[i].first->EvaluateGlobalTransform(help), myMatrix);
			myKey.node = myMatrix;
			myKey.time = myKeyTimes[j];

			myChannel.keys.push_back(myKey);
		}
		animation.channels.push_back(myChannel);
	}


    return true;
}

void FBXLoader::LoadCurve( FbxAnimCurve* curve, std::vector< float >& key_times )
{
    // TODO
	int mySize = curve->KeyGetCount();
	for (int i = 0; i < mySize; i++)
	{
		FbxTime checkTime = curve->KeyGetTime(i);
		float check = (float)checkTime.GetSecondDouble();
		unsigned int KeySize = key_times.size();
		bool found = false;
		for ( unsigned int j = 0; j < KeySize; j++)
		{
			if (check == key_times[j])
			{
				found = true;
				break;
			}
		}

		if (found == false)
			key_times.push_back(check);

	}
}