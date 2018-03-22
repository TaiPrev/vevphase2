#include "tools.h"
#include "avatar.h"
#include "scene.h"

Avatar::Avatar(const std::string &name, Camera * cam, float radius) :
	m_name(name), m_cam(cam), m_walk(false) {
	Vector3 P = cam->getPosition();
	m_bsph = new BSphere(P, radius);
}

Avatar::~Avatar() {
	delete m_bsph;
}

bool Avatar::walkOrFly(bool walkOrFly) {
	bool walk = m_walk;
	m_walk = walkOrFly;
	return walk;
}

// TODO:
//
// AdvanceAvatar: see if avatar can advance 'step' units.

bool Avatar::advance(float step) {
	bool res = true;
	Node *rootNode = Scene::instance()->rootNode(); // root node of scene
	Vector3 original_pos = m_bsph->getPosition();		//se copia la posición original del vector de posición
	Vector3 P = m_cam->getPosition();
	Vector3 dir = -1.0 * m_cam->getDirection();
	if(m_walk){P[0]+= step * dir[0]; P[2]+= step * dir[2];}		
	else{P+= step * dir;}
	m_bsph->setPosition(P);
	if(rootNode->checkCollision(m_bsph)){	//si hay colisión...
		if(m_walk){m_cam->walk(step);}
		else{m_cam->fly(step);}
		//this.m_bsph->set(this.cam->getPosition());	//se reajusta la posición
	}
	else{
		res = false;
		m_bsph->setPosition(original_pos);
	}
	return res;
}

void Avatar::leftRight(float angle) {
	if (m_walk)
		m_cam->viewYWorld(angle);
	else
		m_cam->yaw(angle);
}

void Avatar::upDown(float angle) {
	m_cam->pitch(angle);
}

void Avatar::print() const { }
