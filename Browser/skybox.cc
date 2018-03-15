#include <vector>
#include "skybox.h"
#include "tools.h"
#include "vector3.h"
#include "trfm3D.h"
#include "renderState.h"
#include "gObjectManager.h"
#include "nodeManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"


using std::vector;
using std::string;

static char buffer[4096];

static const char *cmap_faces[6] = { "xpos", "xneg", "ypos", "yneg", "zpos", "zneg" };

static vector<string> generate_names(const string & dirname) {

	static const string ext(".jpg");
	vector<string> names;
	for(size_t i = 0; i < 6; i++) {
		string fname = getFilename(dirname, string(cmap_faces[i]));
		fname.append(".jpg");
		names.push_back(fname);
	}
	return names;
}

GObject *CreateSkyboxGeometry(const string & name,
							  Material *mat) {

	static const int A = 1.0;
	//float A2 = 0.5f * A;
	float A2 =  A;
	TriangleMesh *mesh = new TriangleMesh();

	mesh->addPoint( Vector3(-A, -A, -A));
	mesh->addPoint( Vector3( A, -A, -A));
	mesh->addPoint( Vector3( A, A, -A));
	mesh->addPoint( Vector3(-A, A, -A));
	mesh->addPoint( Vector3(-A, -A,  A));
	mesh->addPoint( Vector3( A, -A,  A));
	mesh->addPoint( Vector3( A, A,  A));
	mesh->addPoint( Vector3(-A, A,  A));

	// normals facing inside

	// bottom
	mesh->addTriangle(0, 4, 5);
	mesh->addTriangle(5, 1, 0);
	// top
	mesh->addTriangle(3, 6, 7);
	mesh->addTriangle(3, 2, 6);
	// front
	mesh->addTriangle(7, 6, 4);
	mesh->addTriangle(6, 5, 4);
	// back
	mesh->addTriangle(2, 3, 1);
	mesh->addTriangle(3, 0, 1);
	// left
	mesh->addTriangle(3, 7, 0);
	mesh->addTriangle(7, 4, 0);
	// right
	mesh->addTriangle(6, 2, 5);
	mesh->addTriangle(2, 1, 5);
	mesh->setMaterial(mat);
	GObject * gobj = GObjectManager::instance()->create(name);
	gobj->add(mesh);
	return gobj;
}


// create a (huge) triangle Mesh with a cube map
Node *CreateSkybox(const std::string &sbname,
				   const std::string &dirname,
				   const std::string &shaderName) {

	// Create cube map texture and assign.
	vector<string> names = generate_names(dirname);
	Texture *ctex = TextureManager::instance()->createCubeMap(sbname,
															  names[0], names[1],
															  names[2], names[3],
															  names[4], names[5]);
	Material *mat = MaterialManager::instance()->create("skybox/", "MG_mat");
	mat->setTexture(ctex);
	GObject *gobj = CreateSkyboxGeometry(getFilename("skybox/", sbname), mat);
	ShaderProgram *skyshader = ShaderManager::instance()->find(shaderName);
	if (!skyshader) {
		fprintf(stderr, "[E] Skybox: no sky shader '%s'\n", shaderName.c_str());
		exit(1);
	}
	// create node
	Node *skynode = NodeManager::instance()->create("MG_SKY");
	skynode->attachShader(skyshader);
	skynode->attachGobject(gobj);
	ASSERT_OPENGL;
	return skynode;
}

void DisplaySky(Node *skynode, Camera *cam) {

	ShaderProgram *prev_shader, *sky_shader;
	GLboolean prev_cull;
	Trfm3D localT;

	RenderState *rs = RenderState::instance();
	sky_shader = skynode->getShader();
	if(!sky_shader) {
		fprintf(stderr, "[E] DisplaySky: sky has no shader\n");
		exit(1);
	}
	glGetBooleanv(GL_CULL_FACE, &prev_cull);
	glDisable(GL_CULL_FACE);
	prev_shader = rs->getShader();
	rs->setShader(sky_shader);
	// move skybox to camera origin
	Vector3 P = cam->getPosition();
	localT.setTrans(P);
	rs->push(RenderState::modelview);
	rs->addTrfm(RenderState::modelview, &localT);
	GObject * sky = skynode->getGobject();
	if(!sky) {
		fprintf(stderr, "[E] DisplaySky: sky has no geometric object\n");
		exit(1);
	}
	GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	sky->draw();
	if (depthTest == GL_TRUE) glEnable(GL_DEPTH_TEST);
	rs->pop(RenderState::modelview);
	// restore shader
	rs->setShader(prev_shader);
	if (prev_cull == GL_TRUE)
		glEnable(GL_CULL_FACE);
}
