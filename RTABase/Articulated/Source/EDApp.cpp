#include "EDApp.h"

#include "ArtLoader.h"
#include "matrix4.h"
#include "vec3.h"

void EDApp::Initialize( )
{
    matrix4f& camera_matrix = m_CameraFrame.GetLocalMat( );

    camera_matrix.make_identity( );
    camera_matrix.axis_pos.y = 0.0f;
    camera_matrix.axis_pos.z = 80.0f;
    camera_matrix.axis_pos.x = 0.0f;

    camera_matrix.axis_z = vec3f( 0.0f, 0.0f, 0.0f ) - camera_matrix.axis_pos;
    camera_matrix.axis_z.normalize( );

    cross_product( camera_matrix.axis_x, vec3f( 0.0f, 1.0f, 0.0f ), camera_matrix.axis_z );
    camera_matrix.axis_x.normalize( );

    cross_product( camera_matrix.axis_y, camera_matrix.axis_z, camera_matrix.axis_x );
    camera_matrix.axis_y.normalize( );

    m_CameraFrame.Update( );

    m_Camera.SetFrame( &m_CameraFrame );

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
 	glEnableClientState(GL_VERTEX_ARRAY);
 	glEnableClientState(GL_NORMAL_ARRAY);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float noLight[] = { 0, 0, 0, 0};
	float allLight[] = { 1, 1, 1, 1};
	float posLight[] = { 0, 0, 0, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, noLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, noLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, allLight);
	glLightfv(GL_LIGHT0, GL_POSITION, posLight);

    // Load the animation
    ArtLoader::Load( "..\\Assets\\megaman.edm", "..\\Assets\\megaman.eda", meshes, m_Animation );

    // Attach the animation to the interpolator
	m_Interpolator.SetAnimation(&m_Animation);
}

void EDApp::Shutdown( )
{
	exit( 0 );
}

void EDApp::Update( )
{    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

        m_Camera.Update( 0.001f );
        m_Camera.ApplyCameraTransform( );

		// Add time to our interpolator
		m_Interpolator.AddTime(0.005f);	// Doesnt have to be time based
		//Interpolate our animation
		m_Interpolator.Process();

		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			glPushMatrix();
			glMultMatrixf(m_Interpolator.GetPose( )[i].ma);

			// Draw the interpolator's current state.
			glVertexPointer(3, GL_FLOAT, 36, &meshes[i].GetVertices()[0].position);
			glNormalPointer(GL_FLOAT, 36, &meshes[i].GetVertices()[0].normal);

			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVertices().size());
			glPopMatrix();
		}

		// Draws the ground.
		float width = 20.0f;
		float y = -30.0f;
		float step = 1.f;
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		for(float x = -width; x <= width; x += step)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for(float z = -width; z <= width; z += step)
			{
				glNormal3f(0.0f, 1.0f, 0.0);
				glVertex3f(x + step, y, z);
				glVertex3f(x, y, z);            
			}
			glEnd();
		}

    glPopMatrix();
}