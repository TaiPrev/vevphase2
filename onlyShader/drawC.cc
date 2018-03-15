/* #include <iostream> */
/* #include <cstdlib> //for exit() */

// gcc -Wall -g -o drawC drawC.c -lm -lglut -lGLU

#include <GL/glut.h>

void display(void);
void Keyboard(unsigned char key, int x, int y);
void DrawCube();
void LookAt();

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize ( 900, 700 );
	glutInitWindowPosition ( 100, 0 );
	// Setup the main window
	glutCreateWindow("OpenGL Example");

	glutKeyboardFunc( Keyboard );
	glutDisplayFunc(display);

	// set OpenGL state values
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glViewport(0, 0, 900, 700);              // Reset The Current Viewport And Perspective Transformation
	// Enable culling

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//Turn Depth Testing On
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f); // Also, sets GLSL fragmen shader gl_DepthRange variable

	// Aliasing

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutMainLoop();
	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//LookAt();
	DrawCube();

	glutSwapBuffers();

}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 27) //escape
		{
			exit(-1);
		}
	else if(key == 'q')
		{
			exit(-1);
		}

	display();
}


struct _BBox {
	float    xMin;
	float    xMax;
	float    yMin;
	float    yMax;
	float    zMin;
	float    zMax;

} thisBBox = { -0.5, 0.5, -0.5, 0.5, -0.75, -0.5 };

void DrawCube()
{

	int i;

	float buffer[24];

	static GLubyte idx[] = {
		0, 1, 2, 2, 3, 0, // front ( 2 triangles: {P0, P1, P2} { P2, P3, P0} )
		5, 4, 7, 7, 6, 5, // back
		4, 0, 3, 3, 7, 4, // left
		1, 5, 6, 6, 2, 1, // right
		3, 2, 6, 6, 7, 3, // top
		4, 5, 1, 1, 0, 4, // bottom
	};

	buffer[0] =  thisBBox.xMin; buffer[1]  = thisBBox.yMin; buffer[2]  = thisBBox.zMax; // P0
	buffer[3] =  thisBBox.xMax; buffer[4]  = thisBBox.yMin; buffer[5]  = thisBBox.zMax; // P1
	buffer[6] =  thisBBox.xMax; buffer[7]  = thisBBox.yMax; buffer[8]  = thisBBox.zMax; // P2
	buffer[9] =  thisBBox.xMin; buffer[10] = thisBBox.yMax; buffer[11] = thisBBox.zMax; // P3
	buffer[12] = thisBBox.xMin + 0.25; buffer[13] = thisBBox.yMin + 0.25; buffer[14] = thisBBox.zMin; // P4
	buffer[15] = thisBBox.xMax + 0.25; buffer[16] = thisBBox.yMin + 0.25; buffer[17] = thisBBox.zMin; // P5
	buffer[18] = thisBBox.xMax + 0.25; buffer[19] = thisBBox.yMax + 0.25; buffer[20] = thisBBox.zMin; // P6
	buffer[21] = thisBBox.xMin + 0.25; buffer[22] = thisBBox.yMax + 0.25; buffer[23] = thisBBox.zMin; // P7

	for(i=0; i < sizeof(idx); i = i + 3) {
		glBegin (GL_TRIANGLES);
		glVertex3f(buffer[3*idx[i    ]], buffer[3*idx[i    ] + 1], buffer[3*idx[i    ] + 2]);
		glVertex3f(buffer[3*idx[i + 1]], buffer[3*idx[i + 1] + 1], buffer[3*idx[i + 1] + 2]);
		glVertex3f(buffer[3*idx[i + 2]], buffer[3*idx[i + 2] + 1], buffer[3*idx[i + 2] + 2]);
		glEnd();
	}
}


void LookAt()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
