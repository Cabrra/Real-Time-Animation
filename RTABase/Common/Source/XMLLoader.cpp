#include "XMLLoader.h"

#include <fstream>

#include "tinyxml.h"
#include "Animation.h"
#include "Mesh.h"
#include "matrix4.h"

bool XMLLoader::LoadMesh( const std::string& file_name, std::vector< Mesh >& meshes )
{
    TiXmlDocument xml_document;
    TiXmlElement* model_element;
    TiXmlElement* mesh_element;
    TiXmlElement* vert_count_element;
    TiXmlElement* poly_count_element;
    TiXmlElement* texture_element;
    TiXmlElement* vert_list_element;
    TiXmlElement* vert_element;
    TiXmlElement* influences_element;
    TiXmlElement* influence_element;
    TiXmlElement* polygon_list_element;
    TiXmlElement* polygon_element;
    TiXmlElement* skeleton_element;
    TiXmlElement* joint_element;
    TiXmlElement* joint_name_element;
    TiXmlElement* joint_parent_element;
    TiXmlElement* joint_transform_element;
    TiXmlElement* axis_element;
    TiXmlElement* joint_children_element;
    TiXmlElement* joint_child_element;

    Mesh mesh;
    int num_verts;
    int num_indices;
    const char* texture_name;
    int vertex_id;
    Mesh::JointInfluence influence;
    int polygon_count;
    int num_joints;
    int joint_index;
    int child_index;
    size_t i, size, parent;
    matrix4f m;

    meshes.clear( );

    if ( xml_document.LoadFile( file_name.c_str( )) == false )
        return false;

    model_element = xml_document.FirstChildElement( "Model" );
    if ( model_element == 0 )
        return false;

    // Get all of the data in the Mesh
    for ( mesh_element = model_element->FirstChildElement( "Mesh" ); mesh_element != 0; mesh_element = mesh_element->NextSiblingElement( "Mesh" ))
    {
        std::string& mesh_name = mesh.GetName( );
        std::vector< Mesh::UniqueVertex >& mesh_vertices = mesh.GetVertices( );
        std::vector< unsigned int >& mesh_indices = mesh.GetIndices( );
        std::vector< Mesh::Joint >& mesh_joints = mesh.GetJoints( );
        std::vector< std::vector< Mesh::JointInfluence >>& mesh_influences = mesh.GetInfluences( );
        std::vector< std::string >& mesh_texture_names = mesh.GetTextureNames( );

        mesh_vertices.clear( );
        mesh_indices.clear( );
        mesh_joints.clear( );
        mesh_influences.clear( );
        mesh_texture_names.clear( );

        mesh_name = mesh_element->Attribute( "name" );

        vert_count_element = mesh_element->FirstChildElement( "VertCount" );
        if ( vert_count_element != 0 )
        {
            num_verts = atoi( vert_count_element->GetText( ));

            mesh_vertices.resize( num_verts );
            mesh_influences.resize( num_verts );
        }

        poly_count_element = mesh_element->FirstChildElement( "PolyCount" );
        if ( poly_count_element != 0 )
        {
            num_indices = atoi( poly_count_element->GetText( )) * 3;

            mesh_indices.resize( num_indices );
        }

        for ( texture_element = mesh_element->FirstChildElement( "Texture" ); texture_element != 0; texture_element = texture_element->NextSiblingElement( "Texture" ))
        {
            texture_name = texture_element->GetText( );

            if ( mesh.LoadTexture( texture_name ) == true )
            {
                mesh_texture_names.push_back( texture_name );
            }
        }

        vert_list_element = mesh_element->FirstChildElement( "vertList" );
        if ( vert_list_element == 0 )
        {
            continue;
        }

        for ( vert_element = vert_list_element->FirstChildElement( "Vert" ); vert_element != 0; vert_element = vert_element->NextSiblingElement( ))
        {
            // Vertex Id
            vertex_id = atoi( vert_element->Attribute( "id" ));

            // Vertex Point
            mesh_vertices[vertex_id].position.x = ( float )atof( vert_element->FirstChildElement( "x" )->GetText( ));
            mesh_vertices[vertex_id].position.y = ( float )atof( vert_element->FirstChildElement( "y" )->GetText( ));
            mesh_vertices[vertex_id].position.z = ( float )atof( vert_element->FirstChildElement( "z" )->GetText( ));

            // Vertex Normal
            mesh_vertices[vertex_id].normal.x = ( float )atof( vert_element->FirstChildElement( "Nx" )->GetText( ));
            mesh_vertices[vertex_id].normal.y = ( float )atof( vert_element->FirstChildElement( "Ny" )->GetText( ));
            mesh_vertices[vertex_id].normal.z = ( float )atof( vert_element->FirstChildElement( "Nz" )->GetText( ));

            // Vertex Texture Coordinate
            mesh_vertices[vertex_id].tex_coord.u = ( float )atof( vert_element->FirstChildElement( "Tu" )->GetText( ));
            mesh_vertices[vertex_id].tex_coord.v = ( float )atof( vert_element->FirstChildElement( "Tv" )->GetText( ));

            // Influences
            influences_element = vert_element->FirstChildElement( "influences" );
            if ( influences_element == 0 )
                continue;

            for ( influence_element = influences_element->FirstChildElement( "influence" ); influence_element != 0; influence_element = influence_element->NextSiblingElement( ))
            {
                // Create a JointInfluence and push it onto the influence list of the current vertex
                influence.joint_index = atoi( influence_element->FirstChildElement( "index" )->GetText( ));
                influence.weight = ( float )atof( influence_element->FirstChildElement( "weight" )->GetText( ));

                mesh_influences[vertex_id].push_back( influence );
            }
        }

        polygon_list_element = mesh_element->FirstChildElement( "polyList" );
        if ( polygon_list_element == 0 )
        {
            continue;
        }

        polygon_count = 0;

        for ( polygon_element = polygon_list_element->FirstChildElement( "Polygon" ); polygon_element != 0; polygon_element = polygon_element->NextSiblingElement( ))
        {
            mesh_indices[polygon_count++] = atoi( polygon_element->FirstChildElement( "i1" )->GetText( ));
            mesh_indices[polygon_count++] = atoi( polygon_element->FirstChildElement( "i2" )->GetText( ));
            mesh_indices[polygon_count++] = atoi( polygon_element->FirstChildElement( "i3" )->GetText( ));
        }

        meshes.push_back( mesh );
    }

    // Get all of the skeleton data
    if ( meshes.empty( ) == false )
    {
        skeleton_element = model_element->FirstChildElement( "Skeleton" );
        if( skeleton_element != 0 )
        {
            std::vector< Mesh::Joint >& mesh_joints = meshes[0].GetJoints( );

            num_joints = atoi( skeleton_element->Attribute( "numBones" ));

            mesh_joints.resize( num_joints );

            for ( joint_element = skeleton_element->FirstChildElement( "bone" ); joint_element != 0; joint_element = joint_element->NextSiblingElement( ))
            {
                joint_name_element = joint_element->FirstChildElement( "name" );

                joint_index = atoi( joint_name_element->Attribute( "index" ));

                // Get joints name
                mesh_joints[joint_index].name = joint_name_element->GetText( );

                joint_parent_element = joint_element->FirstChildElement( "parentIndex" );
                mesh_joints[joint_index].parent = atoi( joint_parent_element->GetText( ));

                // Get the transform
                joint_transform_element = joint_element->FirstChildElement( "localTransform" );
                if ( joint_transform_element )
                {
                    axis_element = joint_transform_element->FirstChildElement( "xAxis" );
                    mesh_joints[joint_index].local_bind_pose_transform.xx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.xy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.xz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.xw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
                    axis_element = joint_transform_element->FirstChildElement( "yAxis" );
                    mesh_joints[joint_index].local_bind_pose_transform.yx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.yy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.yz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.yw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
                    axis_element = joint_transform_element->FirstChildElement( "zAxis" );
                    mesh_joints[joint_index].local_bind_pose_transform.zx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.zy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.zz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.zw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
                    axis_element = joint_transform_element->FirstChildElement( "wAxis" );
                    mesh_joints[joint_index].local_bind_pose_transform.wx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.wy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.wz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
                    mesh_joints[joint_index].local_bind_pose_transform.ww = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
                }

                // Get the children
                joint_children_element = joint_element->FirstChildElement( "children" );
                if ( joint_children_element )
                {
                    for ( joint_child_element = joint_children_element->FirstChildElement( "child" ); joint_child_element != 0;
                        joint_child_element = joint_child_element->NextSiblingElement( ))
                    {
                        child_index = atoi( joint_child_element->Attribute( "index" ));
                        mesh_joints[joint_index].children.push_back( child_index );
                    }
                }
            }

            // Generate world matrices
            size = mesh_joints.size( );
            for ( i = 0; i < size; ++i )
            {
                m.make_identity( );

                for ( parent = i; parent < size; parent = mesh_joints[parent].parent )
                {
                    m = m * mesh_joints[parent].local_bind_pose_transform;
                }

                mesh_joints[i].world_bind_pose_transform = m;
            }
        }
    }

    return true;
}

bool XMLLoader::LoadAnimation( const std::string& file_name, Animation< matrix4f >& animation )
{
    TiXmlDocument xml_document;
    TiXmlElement* animation_element;
    TiXmlElement* duration_element;
    TiXmlElement* bone_element;
    TiXmlElement* key_element;
    TiXmlElement* frame_element;
    TiXmlElement* key_time_element;
    TiXmlElement* transform_element;
    TiXmlElement* axis_element;

    Animation< matrix4f >::Channel channel;
    int num_keys, current_key;

    if ( xml_document.LoadFile( file_name.c_str( )) == false )
        return false;

    animation_element = xml_document.FirstChildElement( "Animation" );
    if ( animation_element == 0 )
        return false;

    // Get Animation Name
    animation.name = animation_element->Attribute( "name" );

    duration_element = animation_element->FirstChildElement( "Duration" );
    if ( duration_element == 0 )
        return false;

    bone_element = animation_element->FirstChildElement( "animBone" );

    for ( bone_element = animation_element->FirstChildElement( "animBone" ); bone_element != 0; bone_element = bone_element->NextSiblingElement( ))
    {
        channel.keys.clear( );

        key_element = bone_element->FirstChildElement( "KeyFrames" );
        if ( key_element == 0 )
            continue;

        num_keys = atoi( key_element->Attribute( "numKeys" ));

        channel.keys.resize( num_keys - 1 );

        frame_element = key_element->FirstChildElement( "keyFrame" );

        for ( frame_element = key_element->FirstChildElement( "keyFrame" ); frame_element != 0; frame_element = frame_element->NextSiblingElement( ))
        {
            current_key = atoi( frame_element->Attribute( "num" )) - 1;
            if ( current_key < 0 )
                continue;

            key_time_element = frame_element->FirstChildElement( "keyTime" );
            if ( key_time_element == 0 )
                continue;

            channel.keys[current_key].time = ( float )atof( key_time_element->GetText( ));

            transform_element = frame_element->FirstChildElement( "worldTransform" );
            if ( transform_element == 0 )
                continue;

            // Get the world transform
            axis_element = transform_element->FirstChildElement( "xAxis" );
            channel.keys[current_key].node.xx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
            channel.keys[current_key].node.xy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
            channel.keys[current_key].node.xz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
            channel.keys[current_key].node.xw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
            axis_element = transform_element->FirstChildElement( "yAxis" );
            channel.keys[current_key].node.yx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
            channel.keys[current_key].node.yy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
            channel.keys[current_key].node.yz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
            channel.keys[current_key].node.yw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
            axis_element = transform_element->FirstChildElement( "zAxis" );
            channel.keys[current_key].node.zx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
            channel.keys[current_key].node.zy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
            channel.keys[current_key].node.zz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
            channel.keys[current_key].node.zw = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
            axis_element = transform_element->FirstChildElement( "wAxis" );
            channel.keys[current_key].node.wx = ( float )atof( axis_element->FirstChildElement( "x" )->GetText( ));
            channel.keys[current_key].node.wy = ( float )atof( axis_element->FirstChildElement( "y" )->GetText( ));
            channel.keys[current_key].node.wz = ( float )atof( axis_element->FirstChildElement( "z" )->GetText( ));
            channel.keys[current_key].node.ww = ( float )atof( axis_element->FirstChildElement( "w" )->GetText( ));
        }

        animation.channels.push_back( channel );
    }

    return true;
}