#include "EDApp.h"

#include "matrix4.h"
#include "vec3.h"
#include "TubeLoader.h"

void DrawGround(void);
void DrawAxes(float fScale);

void EDApp::Initialize( )
{
    matrix4f& camera_matrix = m_CameraFrame.GetLocalMat( );

    camera_matrix.make_identity( );
    camera_matrix.axis_pos.y = 5.0f;
    camera_matrix.axis_pos.z = 10.0f;
    camera_matrix.axis_pos.x = 5.0f;

    camera_matrix.axis_z = vec3f( 5.0f, 0.0f, 0.0f ) - camera_matrix.axis_pos;
    camera_matrix.axis_z.normalize( );

    cross_product( camera_matrix.axis_x, vec3f( 0.0f, 1.0f, 0.0f ), camera_matrix.axis_z );
    camera_matrix.axis_x.normalize( );

    cross_product( camera_matrix.axis_y, camera_matrix.axis_z, camera_matrix.axis_x );
    camera_matrix.axis_y.normalize( );

    m_CameraFrame.Update( );

    m_Camera.SetFrame( &m_CameraFrame );

    GLfloat noLight[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat fullLight[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Set the color that the screen will be cleared with
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc( GL_LEQUAL );

	// Set up our lighting
	// The last value of one makes this a position, instead of a direction
	GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// The light the spot light will point
	GLfloat lightDir[] = {0.0f, 0.0f, -1.0f, 1.0f};
	// Now the three properties of light
	glLightfv(GL_LIGHT0, GL_AMBIENT, noLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fullLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fullLight);
	// Setting the position of the light here will make it and offset from the camera
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Globaly enable lighting
	glEnable(GL_LIGHTING);
	// Enable the one light we are currently using
	glEnable(GL_LIGHT0);
	// Turn off global light
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);	
	// Set up coloring with lighting
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// Set up a shiny material
	glMateriali(GL_FRONT, GL_SHININESS, 20);

	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );

	glEnableClientState( GL_VERTEX_ARRAY );

    TubeLoader::Load( rigid_meshes, rigid_animation, true );
    TubeLoader::Load( smooth_meshes, smooth_animation, false );

	rigid_interpolator.SetAnimation( &rigid_animation );
    smooth_interpolator.SetAnimation( &smooth_animation );

    rigid_deformer.SetMesh( &rigid_meshes[0] );
    smooth_deformer.SetMesh( &smooth_meshes[0] );
}

void EDApp::Shutdown( )
{
	exit( 0 );
}

void EDApp::Update( )
{    
    // Get the elapsed seconds since the last frame
	float fTimeElapsed = m_Timer.GetElapsedTime() * 4.0f;
	m_Timer.Reset();

	// Clear our screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	rigid_interpolator.AddTime( 0.1f );
	rigid_interpolator.Process( );

    smooth_interpolator.AddTime( 0.1f );
	smooth_interpolator.Process( );

    rigid_deformer.Process( rigid_interpolator.GetPose( ));
    smooth_deformer.Process( smooth_interpolator.GetPose( ));

	// Save the identity
	glPushMatrix();
		
		// Update our camera
		m_Camera.Update(fTimeElapsed);
		m_Camera.ApplyCameraTransform();

		// Set LIGHT0's position
		float fPos[4] = { 0.707f, 0.707f, 0.0f, 0.0f };
		glLightfv( GL_LIGHT0, GL_POSITION, fPos );

        glPushMatrix( );
        glTranslatef( 0.0f, 0.0f, 0.0f );

            // Draw the rigid tube
		    glVertexPointer( 3, GL_FLOAT, 0, rigid_deformer.GetSkinVerts( )[0].v );
		    glDisable(GL_LIGHTING);
			    glColor3f( 1.0f, 1.0f, 1.0f );
			    glDrawElements( GL_TRIANGLES, (GLsizei)rigid_meshes[0].GetIndices().size(), GL_UNSIGNED_INT, &rigid_meshes[0].GetIndices()[0] );

			    glEnable( GL_POLYGON_OFFSET_LINE );
			    glPolygonOffset( -1.0f, -1.0f );
			    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			    glColor3f( 0.0f, 0.0f, 0.0f );
			    glDrawElements( GL_TRIANGLES, (GLsizei)rigid_meshes[0].GetIndices().size(), GL_UNSIGNED_INT, &rigid_meshes[0].GetIndices()[0] );
			    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			    glPolygonOffset( 1.0f, 0.0f );
			    glDisable( GL_POLYGON_OFFSET_LINE );
		    glEnable(GL_LIGHTING);

            glDisable( GL_DEPTH_TEST );
		    glDisable( GL_LIGHTING );
		    glLineWidth( 2.0f );
		    for( unsigned int i = 0; i < rigid_interpolator.GetPose( ).size(); ++i )
		    {
			    glPushMatrix();
				    glMultMatrixf( rigid_interpolator.GetPose( )[i].ma );
				    DrawAxes(1.0f);
			    glPopMatrix();
		    }
		    glLineWidth( 1.0f );
		    glEnable( GL_LIGHTING );
		    glEnable( GL_DEPTH_TEST );

        glPopMatrix( );
        glPushMatrix( );
        glTranslatef( 0.0f, 0.0f, -5.0f );

            // Draw the smooth tube
            glVertexPointer( 3, GL_FLOAT, 0, smooth_deformer.GetSkinVerts( )[0].v );
		    glDisable(GL_LIGHTING);
			    glColor3f( 1.0f, 1.0f, 1.0f );
			    glDrawElements( GL_TRIANGLES, (GLsizei)smooth_meshes[0].GetIndices().size(), GL_UNSIGNED_INT, &smooth_meshes[0].GetIndices()[0] );

			    glEnable( GL_POLYGON_OFFSET_LINE );
			    glPolygonOffset( -1.0f, -1.0f );
			    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			    glColor3f( 0.0f, 0.0f, 0.0f );
			    glDrawElements( GL_TRIANGLES, (GLsizei)smooth_meshes[0].GetIndices().size(), GL_UNSIGNED_INT, &smooth_meshes[0].GetIndices()[0] );
			    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			    glPolygonOffset( 1.0f, 0.0f );
			    glDisable( GL_POLYGON_OFFSET_LINE );
		    glEnable(GL_LIGHTING);

            glDisable( GL_DEPTH_TEST );
		    glDisable( GL_LIGHTING );
		    glLineWidth( 2.0f );
		    for( unsigned int i = 0; i < smooth_interpolator.GetPose( ).size(); ++i )
		    {
			    glPushMatrix();
				    glMultMatrixf( smooth_interpolator.GetPose( )[i].ma );
				    DrawAxes(1.0f);
			    glPopMatrix();
		    }
		    glLineWidth( 1.0f );
		    glEnable( GL_LIGHTING );
		    glEnable( GL_DEPTH_TEST );

        glPopMatrix( );

		// Draw the world's coordinate axes
		glDisable(GL_LIGHTING);
			glDepthMask( 0 );
				glColor3f( 0.0f, 0.5f, 0.0f );
				DrawGround();
			glDepthMask( 1 );
		glEnable(GL_LIGHTING);

	// Restore the identity
	glPopMatrix();
}

// Renders a grid
void DrawGround(void)
{
	GLfloat halfSize = 20.0f;
	GLfloat stepSize = 0.5f;
	GLfloat groundLevel = 0.0f;

	glBegin(GL_LINES);
	for(GLfloat x = -halfSize; x <= halfSize; x+= stepSize)
	{
		// These lines horizontal
		glVertex3f(-halfSize, groundLevel, x);
		glVertex3f(halfSize, groundLevel, x);
		// These lines vertical
		glVertex3f(x, groundLevel, -halfSize);
		glVertex3f(x, groundLevel, halfSize);
	}
	glEnd();
}

void DrawAxes(float fScale)
{
	glBegin(GL_LINES);
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( fScale, 0.0f, 0.0f );

		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, fScale, 0.0f );

		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, fScale );
	glEnd();
}