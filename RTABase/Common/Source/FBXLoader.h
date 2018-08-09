#ifndef _FBX_LOADER_H_
#define _FBX_LOADER_H_

#include <vector>
#include <string>
#include <fbxsdk.h>

class Mesh;

template < typename T >
class Animation;

template < typename T >
struct matrix4;
typedef matrix4<float> matrix4f;

////////////////////////////////////////////////////////////////////////////////
// FBXLoader:  A class with static methods to load Mesh and Animation data from
//             an FBX file.
////////////////////////////////////////////////////////////////////////////////
class FBXLoader
{
    // Create a type to represent an array of fbx joints
    typedef std::vector< std::pair< FbxNode*, FbxNodeAttribute* >> JointList;

public:

    ////////////////////////////////////////////////////////////////////////////
    // Load:            Load the mesh and animation information from an FBX
    //                  file.
    // [in]file_name:   The name of the FBX file.
    // [out]meshes:     The meshes that are loaded from the FBX file.
    // [out]animation:  The animatino that is loaded from the FBX file.
    // return:          True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool Load( const std::string& file_name, std::vector< Mesh >& meshes,
        Animation< matrix4f >& animation );

private:

    ////////////////////////////////////////////////////////////////////////////
    // ProcessNode:         Recursive function that traverses the scene and
    //                      loads all the FbxMesh attributes.
    // [in]node:            The current FbxNode being processed.
    // [out]meshes:         The meshes loaded from the scene.
    // [out]unique_joints:  The FBX joints loaded from the scene.  The location
    //                      of the joints in this vector will correlate to the
    //                      indices used in  Mesh::Joint and
    //                      Mesh::JointInfluence.
    // return:              True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool ProcessNode( FbxNode* node, std::vector< Mesh >& meshes,
        JointList& unique_joints );

    ////////////////////////////////////////////////////////////////////////////
    // LoadMesh:      Load a mesh from an FbxMesh attribute.
    // [in]fbx_mesh:  The FbxMesh attribute to load data from.
    // [out]mesh:     The mesh data loaded from the FbxMesh attribute.
    // return:        True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool LoadMesh( FbxMesh* fbx_mesh, Mesh& mesh );

    ////////////////////////////////////////////////////////////////////////////
    // LoadSkin:            Load skin data from an FbxMesh attribute.  This
    //                      method will also load joint data as it is
    //                      encountered in the cluster.
    // [in]fbx_mesh:        The FbxMesh attribute to load data from.
    // [out]mesh:           The mesh data loaded from the FbxMesh attribute.
    // [out]unique_joints:  The FBX joints loaded from the scene.
    // return:              True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool LoadSkin( FbxMesh* fbx_mesh, Mesh& mesh,
        JointList& unique_joints );

    ////////////////////////////////////////////////////////////////////////////
    // LoadTexture:   Load the texture name from a mesh.
    // [in]fbx_mesh:  The FbxMesh attribute to load data from.
    // [out]mesh:     The mesh data loaded from the FbxMesh attribute.
    // return:        True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool LoadTexture( FbxMesh* fbx_mesh, Mesh& mesh );

    ////////////////////////////////////////////////////////////////////////////
    // LoadSkeletonNode:    Load a skeleton node in to our mesh as well as our
    //                      list of unique joints.  The skeleton node is only
    //                      added to the lists if it is unique.  The unique
    //                      joints vector and the meshes joints are paralell
    //                      arrays.
    // [in]node:            The skeleton node to load.
    // [out]mesh:           The mesh to add the joint to ( only if it is
    //                      unique ).
    // [out]unique_joints:  The joint list to add the joint to ( only if it is
    //                      unique ).
    // [out]index:          The index of the joint in both the mesh joint list
    //                      and the unique joint list.  On an error, this will
    //                      be UINT_MAX.
    // Return:              True on success, false on failure.  The only way the
    //                      function can fail is if the node is not a skeleton
    //                      node.
    ////////////////////////////////////////////////////////////////////////////
    static bool LoadSkeletonNode( FbxNode* node, Mesh& mesh,
        JointList& unique_joints, unsigned int& index );

    ////////////////////////////////////////////////////////////////////////////
    // LoadAnimation:      Load an animation from an animation layer.
    // [in]anim_layer:     The animation layer to load.
    // [out]animation:     The loaded animation.
    // [in]unique_joints:  The unique_joints list used for obtaining joint
    //                     indices.
    // Return:             True on success, false on failure.
    ////////////////////////////////////////////////////////////////////////////
    static bool LoadAnimation( FbxAnimLayer* anim_layer, Animation< matrix4f >& animation,
        JointList& unique_joints );

    ////////////////////////////////////////////////////////////////////////////
    // LoadCurve:       Load an animation curve.
    // [in]curve:       The animation curve to use.
    // [out]key_times:  Key times obtained from the animation curve.
    // Return:          void
    ////////////////////////////////////////////////////////////////////////////
    static void LoadCurve( FbxAnimCurve* curve,
        std::vector< float >& key_times );
};

#endif // _FBX_LOADER_H_