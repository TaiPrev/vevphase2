#include "mg.h"
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include "scenes.h"
#include "skybox.h"


// global variables
static float step = 0.5; // advance/retreat step
const static float angle_step = 1.0f * Constants::degree_to_rad; // angular step (rotations, etc)
static Node *displayNode; // the selected node
static Node *skynode = 0;
static int   check_cull = 0;
// Animation settings
// The time in milliseconds between timer ticks
static int MG_TIMERMSECS = 33;
static bool runAnimation = false;
// Global variables for measuring time (in milli-seconds)
static int startTime;
static int prevTime;
static float t;

static void switchAllLights(bool onOff) {
	for(LightManager::iterator it = LightManager::instance()->begin(), end = LightManager::instance()->end();
		it != end; ++it) it->switchLight(onOff);
}

static void switchLight(int ln) {
	assert(ln > 0);
	for(LightManager::iterator it = LightManager::instance()->begin(), end = LightManager::instance()->end();
		ln && it != end; ++it) {
		--ln;
		if (!ln) it->switchLight(!it->isOn());
	}
}

// Init OpenGL rendering context (including rendering window)

static void InitRenderContext(int argc, char** argv,
							  int width, int height,
							  int wposx, int wposy) {

	GLenum glew_err;

	// Init openGL and create a window
	glutInit(&argc, argv);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize ( width, height );
	glutInitWindowPosition ( wposx, wposy );
	// create one window for OpenGL graphics
	glutCreateWindow("browser");

	// Uncomment following line if you have problems initiating GLEW
	//
	// glewExperimental = GL_TRUE;

	glew_err = glewInit();

	if (glew_err != GLEW_OK) {
		fprintf(stderr, "Error when calling glewInit: %s\n", glewGetString(glew_err));
		exit(1);
	}

	// set OpenGL state values
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glViewport(0, 0, width, height);              // Reset The Current Viewport And Perspective Transformation

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Turn Depth Testing On
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f); // Also, sets GLSL fragmen shader gl_DepthRange variable

	// Aliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

void InitLight() {

	Vector3 pos(-10.0, -4.0, -100.0);
	Vector3 dif(0.5, 0.5, 0.5);   //diffuse
	Vector3 spec(0.6, 0.6, 0.6);  //specular

	Vector3 pos_dir(-1, -1, -1);
	Vector3 dif_dir(0.7, 0.7, 0.7);   //diffuse
	Vector3 spec_dir(0.6, 0.6, 0.6);  //specular

	Vector3 pos_dir2(1, 1, 1);

	Vector3 pos_sp(0, 10, 0);
	Vector3 dir_sp(0, 0, -1); // spot direction
	Vector3 spec_sp(0.2, 0.2, 0.2);  //specular
	Vector3 dif_sp(0.5, 0.5, 0.5);   //diffuse
	float cutoff_sp = 10.0;
	float exp_sp = 40;

	LightManager * lmgr = LightManager::instance();

	Light *mainLight = lmgr->create("mainlight", Light::positional);
	mainLight->setPosition(pos);
	mainLight->setDiffuse(dif);
	mainLight->setSpecular(spec);
	mainLight->switchLight(true);

	Light *dirLight = lmgr->create("dirlight", Light::directional);
	dirLight->setPosition(pos_dir);
	dirLight->setDiffuse(dif_dir);
	dirLight->setSpecular(spec_dir);
	dirLight->switchLight(false);

	Light *dirLight2 = lmgr->create("dirlight2", Light::directional);
	dirLight2->setPosition(pos_dir2);
	dirLight2->setDiffuse(dif_dir);
	dirLight2->setSpecular(spec_dir);
	dirLight2->switchLight(false);

	Light *spLight = lmgr->create("spot1", Light::spotlight);
	spLight->setPosition(pos_sp);
	spLight->setDiffuse(dif_sp);
	spLight->setSpecular(spec_sp);
	spLight->setSpotData(dir_sp, cutoff_sp, exp_sp);
	spLight->switchLight(false);

	// Set scene ambient light
	RenderState::instance()->setSceneAmbient(Vector3(0.05, 0.05, 0.05));
}

static void InitCamera(int Width, int Height) {
	PerspectiveCamera *cam = CameraManager::instance()->createPerspective("mainCamera");
	cam->lookAt(Vector3(0.0f, 0.0f, 0.0f),   // position
				Vector3(0.0f, -10.0f, -100.0f),  // look-at
				Vector3(0.0f, 1.0f, 0.0f));  // up vector
	cam->init(30.0f * Constants::degree_to_rad, (float)Width / (float) Height, 0.1f, 2500.0f);
}

static void InitAvatar() {
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera) return; // no main camera
	AvatarManager::instance()->create("avatar", theCamera, 1.0);
}

static void InitShaders() {
	ShaderManager *mgr = ShaderManager::instance();
	mgr->create("dummy", "Shaders/dummy.vert", "Shaders/dummy.frag");
	mgr->create("pervertex", "Shaders/pervertex.vert", "Shaders/pervertex.frag");
	mgr->create("perfragment", "Shaders/perfragment.vert", "Shaders/perfragment.frag");
	mgr->create("bump", "Shaders/bump_shader.vert", "Shaders/bump_shader.frag");
	mgr->create("sky", "Shaders/sky.vert", "Shaders/sky.frag");
}

static void check_cull_camera() {
	CameraManager * mgr = CameraManager::instance();
	Camera *theCamera = mgr->find("mainCamera");
	if (!theCamera) return; // no main camera
	Camera *mapC = mgr->find("mapC");
	if (!mapC) {
		OrthographicCamera *newcam = mgr->createOrthographic("mapC");
		float a = 128.0;
		newcam->init(-a, a, a, -a, 0.1, 1500.0);
		mapC = newcam;
	}
	Vector3 E = theCamera->getPosition();
	Vector3 D = theCamera->getDirection();
	mapC->lookAt(Vector3(E[0], 1.0f, E[2]),
				 Vector3(E[0], 0.0f, E[2]),
				 Vector3(D[0], 0.0f, D[2]));
	RenderState *rs = RenderState::instance();
	rs->loadTrfm(RenderState::projection, mapC->projectionTrfm());
	rs->loadIdentity(RenderState::modelview);
	rs->addTrfm(RenderState::modelview, mapC->viewTrfm());
}

static void Resize(int Width, int Height) {
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera) return; // no main camera
	if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
		Height=1;
	theCamera->onResize(Width, Height);
	glViewport(0, 0, (GLsizei) Width, (GLsizei) Height);
}

static void Render(Camera *theCamera) {

	RenderState *rs = RenderState::instance();
	LightManager *lmgr = LightManager::instance();

	// draw the background color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rs->loadTrfm(RenderState::projection, theCamera->projectionTrfm());
	rs->loadTrfm(RenderState::modelview, theCamera->viewTrfm());
	if (check_cull) check_cull_camera();
	if (skynode) DisplaySky(skynode, theCamera);

	// place lights into scene
	for(LightManager::iterator it = lmgr->begin(), end = lmgr->end();
		it != end; ++it) {
		if(!it->isOn()) continue;
		it->placeScene();
	}

	Scene::instance()->draw();
}

static void Display() {

	Camera *theCamera;
	theCamera = CameraManager::instance()->find("mainCamera");
	if (!theCamera) return; // no main camera

	Scene::instance()->rootNode()->frustumCull(theCamera); // Frustum Culling

	Render(theCamera);
	glutSwapBuffers();
}

// Keyboard dispatcher when ALT key is pressed
static void Keyboard_alt(unsigned char key) {

	static bool line_aliasing = true;
	static bool drawBB = false;
	RenderState *rs;
	Texture *tex;

	switch(key)
		{
		case 'a':
			line_aliasing = !line_aliasing;
			if (line_aliasing) {
				glEnable(GL_LINE_SMOOTH);
			} else {
				glDisable(GL_LINE_SMOOTH);
			}
			break;
		case 'm':
			MaterialManager::instance()->print();
			break;
		case 't':
			TextureManager::instance()->print();
			break;
		case 'c':
			CameraManager::instance()->print();
			break;
		case 'l':
			LightManager::instance()->print();
			break;
		case 'i':
			ImageManager::instance()->print();
			break;
		case 'v':
			RenderState::instance()->top(RenderState::modelview)->print();
			break;
		case 'p':
			RenderState::instance()->top(RenderState::projection)->print();
			break;
		case 's':
			RenderState::instance()->print();
			break;
		case 'b':
			drawBB = !drawBB;
			RenderState::instance()->drawBBoxes(drawBB);
			break;
		case 'f':
			check_cull = 1 - check_cull;
			break;
		case '1':
			displayNode = displayNode->parent();
			break;
		case '2':
			displayNode = displayNode->firstChild();
			break;
		case '3':
			displayNode = displayNode->nextSibling();
			break;
		case '4':
			tex = TextureManager::instance()->find("./obj/cubes/brick.jpg");
			if (tex) {
				tex->cycleMagFilter();
			}
			break;
		case '5':
			tex = TextureManager::instance()->find("./obj/cubes/brick.jpg");
			if (tex) {
				tex->cycleMinFilter();
			}
			break;
		}
	glutPostRedisplay( );
}


// General keyboard dispatcher
static void Keyboard (unsigned char key, int x, int y) {

	static size_t i = 0;
	static bool avatar_mode = false;
	int key_mod;
	Avatar *theAvatar;
	Camera *theCamera;
	float fovy;

	key_mod = glutGetModifiers();
	if (key_mod == GLUT_ACTIVE_ALT) {
		// If ALT key pressed, call Keyboard_alt and exit
		Keyboard_alt(key);
		return;
	}

	switch(key)
		{
		case '0':
			runAnimation = !runAnimation;
			break;
		case '1':
			switchLight(1);
			break;
		case '2':
			switchLight(2);
			break;
		case '3':
			switchLight(3);
			break;
		case '4':
			switchLight(4);
			break;
		case '5':
			switchLight(5);
			break;
		case '6':
			switchLight(6);
			break;
		case '7':
			switchLight(7);
			break;
		case 's':
			// Enable Shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 'S':
			// Disable Shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 'l':
			// Turn on all lights
			switchAllLights(true);
			break;
		case 'L':
			// Disable Lighting
			switchAllLights(false);
			break;
		case 'z':
			glEnable(GL_CULL_FACE);
			break;
		case 'Z':
			glDisable(GL_CULL_FACE);
			break;

		case '.':
			theAvatar = AvatarManager::instance()->find("avatar");
			if (theAvatar) {
				avatar_mode = !avatar_mode;
				theAvatar->walkOrFly(avatar_mode);
			}
			break;
		case 'a':
			displayNode->rotateY(-angle_step);
			break;
		case 'd':
			displayNode->rotateY(angle_step);
			break;
		case 'w':
			displayNode->rotateX(-angle_step);
			break;
		case 'x':
			displayNode->rotateX(angle_step);
			break;
		case 'i':
			displayNode->translate(Vector3(0.0, 0.0, 2.0*-step));
			break;
		case 'k':
			displayNode->translate(Vector3(0.0, 0.0, 2.0*step));
			break;
		case 'f':
			theCamera = CameraManager::instance()->find("mainCamera");
			if (theCamera) {
				theCamera->zoom(std::max( 0.01f, theCamera->getZoom() - 0.01f));
			}
			break;
		case 'F':
			theCamera = CameraManager::instance()->find("mainCamera");
			if (theCamera) {
				theCamera->zoom(std::min( 2.0f * Constants::pi, theCamera->getZoom() + 0.01f));
			}
			break;
		case 27: // ESC
			exit(0);
			break;
		}
	glutPostRedisplay( );
}

// Special keyboard dispatcher (arrow keys, etc).
static void SpecialKey (int key, int x, int y) {

	Camera *theCamera;
	Avatar *theAvatar;

	theAvatar = AvatarManager::instance()->find("avatar");
	if (!theAvatar) return;

	switch(key)
		{
		case GLUT_KEY_RIGHT:
			theAvatar->leftRight(-0.05f);
			break;
		case GLUT_KEY_LEFT:
			theAvatar->leftRight(0.05f);
			break;
		case GLUT_KEY_UP:
			theAvatar->upDown(0.05f);
			break;
		case GLUT_KEY_DOWN:
			theAvatar->upDown(-0.05f);
			break;
		case GLUT_KEY_PAGE_UP:
			theAvatar->advance(1);
			break;
		case GLUT_KEY_PAGE_DOWN:
			theAvatar->advance(-1);
			break;
		case GLUT_KEY_HOME:
			theCamera = CameraManager::instance()->find("mainCamera");
			if (theCamera) theCamera->goLast();
			break;
		}
	glutPostRedisplay( );
}

void mouseClick(int button, int state,
				int x, int y) {
}

void mouse(int x, int y) {
	// lower left if (0,0)
	Camera *theCamera = CameraManager::instance()->find("mainCamera");
}

void idle(void) {
}

void animate(int value) {
	RenderState *rs = RenderState::instance();
	//int direction = 1;
	// Set up the next timer tick (do this first)
	glutTimerFunc(MG_TIMERMSECS, animate, 0);

	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;

	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####
	if (runAnimation) {
		// Force a redisplay to render the new image
			t = t + 0.01f;
			if(t>1.0f){t=0.0f;}
		/*
		else{
			t -= 0.01f;
			if(t<=0.0f){direction = 1; printf("%d\n", direction);}
		}*/
		printf("time %5.2f\n", t);

		rs->setTime(t);

		glutPostRedisplay();
	}
	// ##### END OF GAME/APP MAIN CODE #####
	prevTime = currTime;
}


int main(int argc, char** argv) {

	srand(time(0));
	InitRenderContext(argc, argv, 900, 700, 100, 0);
	// set GLUT callback functions
	glutDisplayFunc( Display );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKey );
	glutReshapeFunc( Resize );
	glutMouseFunc( mouseClick );
	glutMotionFunc( mouse );
	//glutIdleFunc( idle );

	if (argc == 2) {
		// load scene from JSON scene
		displayNode = parse_scene(argv[1]);
	} else {
		// regular scene
		InitCamera(900, 700);
		InitAvatar();
		InitLight();
		InitShaders();
		// Change the line below for different scenes
		displayNode = create_scene();
		// Other possible scenes:
		//
		// displayNode = create_scene_city();
	}

	Scene::instance()->attach(displayNode);
	// Start the timer (uncomment if you want animations)
	glutTimerFunc(MG_TIMERMSECS, animate, 0);

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	CreateSkybox("skydome", "obj/sky", "sky");
	//skynode = NodeManager::instance()->find("MG_SKY");
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glutMainLoop();

	return 0;
}
