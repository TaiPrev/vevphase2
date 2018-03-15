#include <cmath>
#include <string>
#include <vector>
#include <set>
#include "tools.h"
#include "trfm3D.h"
#include "scene.h"
#include "nodeManager.h"
#include "gObjectManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"
#include "lightManager.h"
#include "cameraManager.h"

using std::set;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;


static Node *create_city (int N, std::vector<GObject *> & gObj_list); // forward declaration
static Node *create_floor_city(const string &, const string &);

Node *create_scene() {

	GObject *gobj;
	GObject *gobj2;
	GObject *gobj3;
	GObject *gobj4;
	GObject *gobj5;
	Node    *myNode, *auxNode;

	static Trfm3D TT;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	// sceneRegisterGObject takes ownership of geometric objects
	//gobj = gmgr->createFromWavefront( "./obj/spheres/", "solid.obj");
	gobj =  gmgr->createFromWavefront( "./obj/spheres/", "smooth.obj");
	gobj2 = gmgr->createFromWavefront( "./obj/chapel/", "chapel.obj");
	gobj3 = gmgr->createFromWavefront( "./obj/cubes/", "cubo.obj");
	gobj4 = gmgr->createFromWavefront( "./obj/cubes/", "cubotex.obj");
	gobj5 = gmgr->createFromWavefront( "./obj/floor/", "floor.obj");

	TT.setTrans(Vector3(0, -10, -100));
	myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	Scene::instance()->attach(myNode); // takes ownership
	myNode->attachShader(ShaderManager::instance()->find("pervertex"));
	myNode->setDrawBBox(false);

	TT.setTrans(Vector3( -20, 0, 0));
	auxNode = nmgr->create("smooth");
	auxNode->setTrfm(&TT);
	auxNode->attachGobject(gobj);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 20, 0, 0));
	auxNode = nmgr->create("chapel");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj2);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, 0, -20));
	auxNode = nmgr->create("cubo");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj3);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, 0, 20));
	auxNode = nmgr->create("cubotex");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj4);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, -10, 00));
	auxNode = nmgr->create("floor");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj5);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	return myNode;
}

Node *create_scene_bmap() {

	GObject *gobj;
	GObject *gobj2;
	GObject *gobj3;
	GObject *gobj4;
	GObject *gobj5;
	Node    *myNode, *auxNode;

	static Trfm3D TT;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	// sceneRegisterGObject takes ownership of geometric objects
	//gobj = gmgr->createFromWavefront( "./obj/spheres/", "solid.obj");
	gobj =  gmgr->createFromWavefront( "./obj/spheres/", "smooth.obj");
	gobj2 = gmgr->createFromWavefront( "./obj/chapel/", "chapel.obj");
	gobj3 = gmgr->createFromWavefront( "./obj/cubes/", "cubo.obj");
	gobj4 = gmgr->createFromWavefront( "./obj/cubes/", "cubotex.obj");
	gobj5 = gmgr->createFromWavefront( "./obj/floor/", "waterfloor.obj");

	TT.setTrans(Vector3( 0, -10, -100));
	myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	Scene::instance()->attach(myNode); // takes ownership
	myNode->attachShader(ShaderManager::instance()->find("pervertex"));
	myNode->setDrawBBox(false);

	TT.setTrans(Vector3( -20, 0, 0));
	auxNode = nmgr->create("smooth");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 20, 0, 0));
	auxNode = nmgr->create("chapel");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj2);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, 0, -20));
	auxNode = nmgr->create("cubo");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj3);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, 0, 20));
	auxNode = nmgr->create("cubotex");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj4);
	auxNode->setDrawBBox(false);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 0, -10, 00));
	auxNode = nmgr->create("waterflow");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj5);
	auxNode->setDrawBBox(false);
	auxNode->attachShader(ShaderManager::instance()->find("bump"));
	myNode->addChild(auxNode); // takes ownership

	return myNode;
}

Node *create_scene_city() {

	// casita3/house01
	// casa5/wachhaus
	// dom/dom

	std::vector<GObject *> gObj_list;
	static Trfm3D TT;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	gObj_list.push_back(gmgr->createFromWavefront( "./obj/casita3/", "house01.obj"));
	gObj_list.push_back(gmgr->createFromWavefront( "./obj/casa5/", "wachhaus.obj"));
	gObj_list.push_back(gmgr->createFromWavefront( "./obj/dom/", "dom.obj"));

	Node *city = create_city(500, gObj_list);

	TT.setTrans(Vector3( 0, -5, 00));
	//TT.setTrans(Vector3( 0, 0, 0));
	Node *myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	myNode->attachShader(ShaderManager::instance()->find("pervertex"));
	Scene::instance()->attach(myNode);
	myNode->addChild(city);

	Node *floor = create_floor_city( "./obj/floor/", "cityfloor.obj");
	myNode->addChild(floor); // takes ownership
	return myNode;
}

Node *create_scene_city_bmap() {

	// casita3/house01
	// casa5/wachhaus
	// dom/dom

	vector<GObject *> gObj_list;
	static Trfm3D TT;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	gObj_list.push_back(gmgr->createFromWavefront( "./obj/casita3/", "house01.obj"));
	gObj_list.push_back(gmgr->createFromWavefront( "./obj/casa5/", "wachhaus.obj"));
	gObj_list.push_back(gmgr->createFromWavefront( "./obj/dom/", "dom.obj"));

	Node *city = create_city(500, gObj_list);

	TT.setTrans(Vector3( 0, -5, 00));
	//TT.setTrans(Vector3( 0, 0, 0));
	Node *myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	myNode->attachShader(ShaderManager::instance()->find("pervertex"));
	Scene::instance()->attach(myNode);
	myNode->addChild(city);

	Node *floor = create_floor_city( "./obj/floor/", "cityfloor_grass.obj");
	floor->attachShader(ShaderManager::instance()->find("bump"));
	myNode->addChild(floor); // takes ownership
	return myNode;
}


Node *create_scene_noT() {

	GObject *gobj;
	Trfm3D TT;
	Node *myNode, *auxNode;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	gobj = gmgr->createFromWavefront( "./obj/chapel/", "chapel_I.obj");

	TT.setTrans(Vector3( 0, 0, 0));
	myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	myNode->attachShader(ShaderManager::instance()->find("dummy"));
	Scene::instance()->attach(myNode);

	TT.setTrans(Vector3( 1, 0, 0));
	auxNode = nmgr->create("chapel1");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj);
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( -1, 0, 0));
	auxNode = nmgr->create("chapel2");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gobj);
	myNode->addChild(auxNode); // takes ownership

	return myNode;
}

Node *create_scene_triang() {

	// sceneRegisterGObject takes ownership of geometric objects
	//gobj = gmgr->createFromWavefront( "./obj/cubes/", "triangle.obj");

	/*   auxNode = nmgr->createGobj(gobj, &T); */
	/*   sceneAttachNode(auxNode); // takes ownership */

	static Trfm3D TT;
	Node *myNode, *auxNode;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	TT.setTrans(Vector3( 0, -10, -100));
	myNode = nmgr->create("root");
	myNode->setTrfm(&TT);
	Scene::instance()->attach(myNode);

	TT.setTrans(Vector3( -5, 0, 0));
	auxNode = nmgr->create("triangle");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gmgr->createFromWavefront( "./obj/cubes/", "triangle.obj"));
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 5, 0, 0));
	auxNode = nmgr->create("quad");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gmgr->createFromWavefront( "./obj/cubes/", "quad.obj"));
	myNode->addChild(auxNode); // takes ownership

	TT.setTrans(Vector3( 10, 0, 0));
	auxNode = nmgr->create("chapel");
	auxNode->setTrfm( &TT);
	auxNode->attachGobject(gmgr->createFromWavefront( "./obj/chapel/", "chapel.obj"));
	auxNode->attachShader(ShaderManager::instance()->find("pervertex"));
	myNode->addChild(auxNode); // takes ownership

	return myNode;
}

/////////////////////////// Create city functions

struct coord2d {
	int x;
	int y;
};

static set<pair<float, float> > gen_coords(size_t maxX, size_t maxY, size_t N) {
	set<pair<float, float> > coords;
	size_t inserted = 0;
	while(inserted < N) {
		if(coords.insert(make_pair(rand() % maxX, rand() % maxY)).second)
			inserted++;
	}
	return coords;
}

static string house_name(const string &base, int x, int y) {
	return base + string("#") + float_to_string(x) + string("#") + float_to_string(y);
}

static Node *create_city (int N, vector<GObject *> & gObj_list) {

	size_t maxX = 200;
	size_t maxY = 200;
	float bbsize = 5.0f;

	// elements have occupy ~ (5x5) units

	NodeManager *nmgr = NodeManager::instance();
	// generate coords (without repetition)
	set<pair<float, float> > coords = gen_coords(maxX, maxY, N);
	// Create Nodes
	pair<float, float> coord_center;
	coord_center.first = (int)(floorf((float) maxX / 2.0f));
	coord_center.second = (int)(floorf((float) maxY / 2.0f));
	Trfm3D placement;
	Node *root = nmgr->create("cityroot");
	root->setTrfm(&placement);
	for(set<pair<float, float> >::iterator it = coords.begin(), end = coords.end();
		it != end; ++it) {
		float placeX = (it->first - coord_center.first) * bbsize;
		float placeY = (coord_center.second - it->second) * bbsize;
		placement.setTrans(Vector3(placeX, 0,placeY));
		Node *auxNode = nmgr->create(house_name("house", placeX, placeY));
		auxNode->setTrfm(&placement);
		auxNode->attachGobject( gObj_list[ rand() % gObj_list.size()] ); // get one gObj at random
		root->addChild(auxNode);
	}
	return root;
}

static Node *create_floor_city(const string & dir, const string &fname) {

	static Trfm3D TT;
	static float floorsize = 108.0;
	int i, j;
	int N = 12; // 5x5 patches
	float left, up;
	float x, z;

	GObjectManager *gmgr = GObjectManager::instance();
	NodeManager *nmgr = NodeManager::instance();

	GObject *gobj = gmgr->createFromWavefront(dir, fname);
	Node *myNode = nmgr->create("cityfloor");
	TT.setTrans(Vector3( 0.0, -1.5, 00));
	myNode->setTrfm(&TT);

	left = -1.0f * floorsize * (float) N / 2.0f;

	for (i = 0; i < N; i++) {
		x = left + floorsize * i;
		for(j = 0; j < N; j++) {
			z = left + floorsize * j;
			TT.setTrans(Vector3( x, 0.0, z));
			Node *aux = nmgr->create(house_name("floor", x, z));
			aux->setTrfm(&TT);
			aux->attachGobject(gobj);
			aux->setDrawBBox(false);
			myNode->addChild(aux); // takes ownership
		}
	}
	return myNode;
}
