#include "EDApp.h"

#include "GLee.h"
#include <gl/glu.h>

#include "AppConfig.h"
#include "FBXLoader.h"

EDApp::EDApp( )
{
}

EDApp::~EDApp( )
{
}

EDApp& EDApp::GetInstance( )
{
    static EDApp instance;
    return instance;
}

void EDApp::Initialize( )
{
    matrix4f& camera_matrix = camera_frame.GetLocalMat( );

    camera_matrix.make_identity( );
    camera_matrix.axis_pos.y = 1.0f;
    camera_matrix.axis_pos.z = 8.0f;
    camera_matrix.axis_pos.x = 0.0f;

    camera_matrix.axis_z = vec3f( 0.0f, 0.0f, 0.0f ) - camera_matrix.axis_pos;
    camera_matrix.axis_z.normalize( );

    cross_product( camera_matrix.axis_x, vec3f( 0.0f, 1.0f, 0.0f ), camera_matrix.axis_z );
    camera_matrix.axis_x.normalize( );

    cross_product( camera_matrix.axis_y, camera_matrix.axis_z, camera_matrix.axis_x );
    camera_matrix.axis_y.normalize( );

    camera_frame.Update( );

    camera.SetFrame( &camera_frame );

    timer.Reset( );

    // Set the color that the screen will be cleared with
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    // Enable texturing
    glEnable( GL_TEXTURE_2D );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    // Set up our lighting
    GLfloat no_light[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat full_light[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_pos[4] = { 0.70710678f, 0.70710678f, 0.0f, 0.0f };
    GLfloat light_dir[4] = { 0.0f, 0.0f, -1.0f, 1.0f };

    // Set the three light properties
    glLightfv( GL_LIGHT0, GL_AMBIENT, no_light );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, full_light );
    glLightfv( GL_LIGHT0, GL_SPECULAR, full_light );
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );

    // Globaly enable lighting
    glEnable( GL_LIGHTING );

    // Enable the one light we are currently using
    glEnable( GL_LIGHT0 );

    // Set up coloring with lighting
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

    // Set up a shiny material
    glMateriali( GL_FRONT, GL_SHININESS, 20 );

    glCullFace( GL_BACK );
    glEnable( GL_CULL_FACE );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    FBXLoader::Load( "..\\Assets\\Robot_Walk.fbx", meshes, animation );

    interpolator.SetAnimation( &animation );
    deformer.SetMesh( &meshes[0] );
}

void EDApp::Update( )
{
    float elapsed_time;

    // Get the elapsed seconds since the last frame
    elapsed_time = timer.GetElapsedTime( );
    timer.Reset( );

    // Clear our screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Save the identity
    glPushMatrix( );

    // Update our camera
    camera.Update( elapsed_time * 2.0f );
    camera.ApplyCameraTransform( );

    // Update our interpolator
    interpolator.AddTime( elapsed_time );
    interpolator.Process( );

    if ( meshes[0].GetVertices( ).size( ) > 0 )
    {
        deformer.Process( interpolator.GetPose( ));

        glPushMatrix( );

        glVertexPointer( 3, GL_FLOAT, 0, deformer.GetSkinVerts( )[0].v );
        //glVertexPointer( 3, GL_FLOAT, 36, &meshes[0].GetVertices( )[0].position.x );
        glNormalPointer( GL_FLOAT, 36, &meshes[0].GetVertices( )[0].normal.x );
        glTexCoordPointer( 2, GL_FLOAT, 36, &meshes[0].GetVertices( )[0].tex_coord.u );

        glColor3f( 1.0f, 1.0f, 1.0f );

        glDrawElements( GL_TRIANGLES, ( GLsizei )meshes[0].GetIndices( ).size( ), GL_UNSIGNED_INT,
            &meshes[0].GetIndices( )[0] );

        glEnable( GL_POLYGON_OFFSET_LINE );
        glPolygonOffset( -1.0f, -1.0f );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glPolygonOffset( 1.0f, 0.0f );
        glDisable( GL_POLYGON_OFFSET_LINE );

        glPopMatrix( );
    }

    // Draw joint axes
    /*glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glLineWidth( 2.0f );

    for ( unsigned int i = 0; i < meshes[0].GetJoints().size( ); ++i )
    {
        glPushMatrix( );
		glMultMatrixf(meshes[0].GetJoints()[i].world_bind_pose_transform.ma);
        DrawAxes( );
        glPopMatrix( );
    }
    
    glLineWidth( 1.0f );
    glEnable( GL_LIGHTING );
    glEnable( GL_DEPTH_TEST );*/

    // Draw the ground
    glDisable( GL_LIGHTING );
    glDepthMask( 0 );
    glColor3f( 0.0f, 0.5f, 0.0f );
    DrawGround( );
    glDepthMask( 1 );
    glEnable( GL_LIGHTING );

    // Restore the identity
    glPopMatrix( );
}

void EDApp::Shutdown( )
{
}

EDCamera& EDApp::GetCamera( )
{
    return camera;
}

void EDApp::DrawGround( )
{
    const GLfloat half_size = 20.0f;
    const GLfloat step_size = 0.5f;
    const GLfloat ground_level = 0.0f;
    GLfloat x;

    glBegin( GL_LINES );

    for ( x = -half_size; x <= half_size; x+= step_size )
    {
        // Draw horizontal lines
        glVertex3f( -half_size, ground_level, x );
        glVertex3f( half_size, ground_level, x );

        // Draw vertical lines
        glVertex3f( x, ground_level, -half_size );
        glVertex3f( x, ground_level, half_size );
    }

    glEnd( );
}

void EDApp::DrawAxes( float scale )
{
    glBegin( GL_LINES );

    glColor3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( scale, 0.0f, 0.0f );

    glColor3f( 0.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, scale, 0.0f );

    glColor3f( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, scale );

    glEnd( );
}