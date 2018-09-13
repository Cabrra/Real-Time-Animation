Real Time Animation Labs
========================

## Overview

![real time](https://raw.githubusercontent.com/Cabrra/cabrra.github.io/master/Images/RTA.png?token=AI_RbbhWLN4jHByI_MnQaei-XPRDVZZoks5bo1U1wA%3D%3D)

- Milestone one: enforce understanding of different animation 
	+ Vertex Morphing
		+ Load an Animation object
			+ Implement VertLoader::Load
			
		+ Set up the Interpolator object
			+ Implement Interpolator::ProcessKeyframes function being able to support reverse animation
			+ Implement Interpolator::SetTime function
			+ Implement Interpolator::SetAnimation function
		
	+ Articulated
		+ Load Mesh objects and Animation object
			+ Implement ArtLoader::Load function
		
		+ Modify interpolator to work with channels
			+ Implement Interpolator::ProcessChannels
		
	+ Skinning
	The Deformer::Process function handles the processing of both rigid and smooth skinning. Rigid skinning is considered a special case of smooth skinning, where each vertex is only affected by one joint at a weight of 1.0. In other words, the Deformer::Process function does not make a distinction between rigid and smooth skinning.
		+ Implement Deformer::Process function
		+ Implement Deformer::OrthoNormalInverse function
		+ Implement Deformer::Multiply function

- Milestone two: working with an external API (FBX)
	+ FBXLoader
		+ ProcessNode: This function is the function used to traverse the scene. It is implemented as a pre-order depth first traversal. We are only interested in processing nodes that have a mesh attribute. If a node has a mesh attribute, we call FBXLoader::LoadMesh, FBXLoader::LoadSkin, and FBXLoader::LoadTexture.
		
		+ LoadSkin: This function is used to load skinning information. Primarily is filling out Mesh::influences. It also fills out Mesh::joints as we encounter the joints in the skin and loops through the number of skin deformers in the FbxMesh. For each skin deformer, it loops through the skin clusters. Each skin cluster represents one joints effect on a cluster of control points and the cluster's link represents the joint. The call to FBXLoader::LoadSkeletonNode is made using the cluster link ( joint ). After calling FBXLoader::LoadSkeletonNode, we calculate the world bind pose transform of the joint. This is obtained by multiplying the cluster transform matrix by the cluster transform link matrix. Note, the conversion from FbxAMatrix to a matrix4f is achieved by copying each element from one to the other. After the joint information is loaded, influence data from the cluster is loaded. Each control point index in the cluster is used to fill out a Mesh::JointInfluence. The joint index is known from the call to FBXLoader::LoadSkeletonNode.
		
		+ LoadSkeletonNode: This function loads a skeleton node into the mesh joint list as well as the unique joint list. The mesh joint list and unique joint list are parallel arrays. A skeleton node is added to the lists only if it is unique. If the skeleton node does not have a skeleton attribute, the function returns false. In all other cases, the function returns true. looking through the unique joints list determines if this skeleton node is unique. If the skeleton node already exists, it just return the index of the existing joint. If the skeleton node is unique, it is added to the mesh joint list and unique joint list. Note, this function is using lazy initialization to recurse over the Parent/Children indices and fill the unique joints.
		
		+ LoadAnimation: The function loads animation information from an animation layer, looping through each unique joint. Each unique joint has translation, rotation, and scaling properties. Calling LoadCurve for each of the 9 curves builds a list of the unique key times. Note that when we loo through all key times we exclude the first, because it represents the bind pose in the model we are importing.  
		
		+ LoadCurve: load unique key times from a curve.
	
	+ Animation Blender
		+ Process: this fills out the "pose" attribute. If the transition time is less than or equal to zero, we are NOT blending -> fill out the pose from the current interpolator. If we are blending, we run the current and next interpolators, and blend between the results based on how much of the transition time has elapsed.

## Controls

+ Some labs accept key input: AWSD

## Built With

* [Visual Studio 2013](https://visualstudio.microsoft.com/vs/older-downloads/) - C++ development win32 application
* [FBX SDK 2014](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2019-0)
* DirectX 11
* [OpenGL](https://sites.fas.harvard.edu/~lib175/pages/visstudio.html) - installation guide

## Contributing

Please read [CONTRIBUTING.md](https://github.com/Cabrra/Contributing-template/blob/master/Contributing-template.md) for details on the code of conduct, and the process for submitting pull requests to me.

## Authors

* **Kyle Turco**            - Course Director
* **Jagoba "Jake" Marcos** 	- Student implementation -[Cabrra](https://github.com/Cabrra)

## License

This project is licensed under the MIT license - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Full Sail University - Game Development Department
