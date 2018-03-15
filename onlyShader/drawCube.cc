#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "shaderUtils.h"

#define ASSERT_OPENGL { GLenum errorCode = glGetError();				\
		if (errorCode != GL_NO_ERROR) {									\
			fprintf (stderr, "[E] %s, line %d. Error: %s\n", __FUNCTION__, __LINE__, gluErrorString(errorCode)); \
			exit(1);}}

GLuint vShader, fShader, program;

float M[16] = {1, 0, 0, 0,
			   0, 1, 0, 0,
			   0, 0, 1, 0,
			   0, 0, 0, 1};

float P[16] = {1, 0, 0, 0,
			   0, 1, 0, 0,
			   0, 0, 1, 0,
			   0, 0, 0, 1};

GLint unifM;
GLint unifP;
GLuint vertex_vbo = 0;
GLuint index_vbo = 0;

void InitShaders() {

	int errorCode;

	vShader = LoadShader(GL_VERTEX_SHADER, "dummy.vert");
	fShader = LoadShader(GL_FRAGMENT_SHADER, "dummy.frag");

	program = CreateProgramFromObjects(vShader, fShader, &errorCode);
	if (errorCode) {
		exit(1);
	}
	unifM = GetProgramUniform("dummy", program, "modelToCameraMatrix");
	unifP = GetProgramUniform("dummy", program, "cameraToClipMatrix");
}

struct _BBox {
	float    xMin;
	float    xMax;
	float    yMin;
	float    yMax;
	float    zMin;
	float    zMax;

} thisBBox = { -0.5, 0.5, -0.5, 0.5, -0.75, -0.5 };


void ReleaseObj() {
	if (vertex_vbo) glDeleteBuffers(1, &vertex_vbo);
	if (index_vbo) glDeleteBuffers(1, &index_vbo);
}

void InitObj() {

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

	// create new VBO for vertices
	glGenBuffers(1, &vertex_vbo);
	ASSERT_OPENGL;
	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	ASSERT_OPENGL;
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(buffer),
				 &buffer[0],
				 GL_STATIC_DRAW);
	ASSERT_OPENGL;
	// create new VBO for indices
	glGenBuffers(1, &index_vbo);
	// bind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	ASSERT_OPENGL;
	// upload data to VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(idx),
				 &idx[0],
				 GL_STATIC_DRAW);
	ASSERT_OPENGL;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Display() {

	// draw the background color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	shader_set_uniform_matrix4(unifM, &M[0]);
	shader_set_uniform_matrix4(unifP, &P[0]);

	// Documentation of glBindBuffer()
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	ASSERT_OPENGL;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	ASSERT_OPENGL;

	// Attribute specification
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position
	ASSERT_OPENGL;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	ASSERT_OPENGL;

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
	ASSERT_OPENGL;

	glDisableVertexAttribArray(0);
	ASSERT_OPENGL;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);

	glutSwapBuffers();
}

// General keyboard dispatcher
static void Keyboard (unsigned char key, int x, int y) {

	switch(key)
		{
		case 27: // ESC
			exit(0);
			break;
		}
	glutPostRedisplay( );
}



int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize ( 900, 700 );
	glutInitWindowPosition ( 100, 0 );
	glutCreateWindow("drawCube");

	// set OpenGL state values
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	// Reset The Current Viewport And Perspective Transformation
	glViewport(0, 0, 900, 700);
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

	glewInit();
	InitShaders();
	InitObj();

	glutDisplayFunc( Display );
	glutKeyboardFunc( Keyboard );
	glutMainLoop();
	ReleaseObj();
	return 0;
}
