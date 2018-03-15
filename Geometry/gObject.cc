#include <iterator>
#include <cstdio>
#include <string>
#include "tools.h"
#include "gObject.h"
#include "triangleMeshGL.h"

using std::list;
using std::string;

GObject::GObject(const std::string & name) : m_name(name) {}
GObject::~GObject() {
	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		delete *it;
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		delete *it;
	}
}

GObject *GObject::createFromWavefront(const string & DirName, const string & FileName ) {

	GObject *newGObject;
	TriangleMesh *oneMesh;
	list<TriangleMesh *> auxlist;

	newGObject = new GObject(getFilename(DirName, FileName));
	TriangleMesh::CreateTMeshObj(DirName, FileName, auxlist);
	for(list<TriangleMesh *>::iterator it = auxlist.begin(), end = auxlist.end();
		it != end; ++it) {
		newGObject->add(*it);
	}
	return newGObject;
}

void GObject::add(TriangleMesh *oneMesh) {

	if(oneMesh->getMaterial()->isTransp()) {
		m_meshes_transp.push_back(oneMesh);
	} else {
		m_meshes.push_back(oneMesh);
	}
	updateContainer();
}

const std::string & GObject::getName() const { return m_name; }

void GObject::draw() {
	drawOpaque();
	drawTransparent();
}

void GObject::drawOpaque() {
	for(list<TriangleMesh *>::const_iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		TriangleMeshGL::draw(*it);
	}
}

void GObject::drawTransparent() {
	for(list<TriangleMesh *>::const_iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		TriangleMeshGL::draw(*it);
	}
}

const BBox * GObject::getContainer() {
	return &m_container;
}

void GObject::updateContainer() {

	TriangleMesh  *oneMesh;
	float    xmin, xmax, ymin, ymax, zmin, zmax;

	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		(*it)->includeBBox(m_container);
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		(*it)->includeBBox(m_container);
	}
}

// apply a trfm to all meshes
void GObject::applyTrfm(const Trfm3D * trfm) {
	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		(*it)->applyTrfm(trfm);
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		(*it)->applyTrfm(trfm);
	}
	updateContainer();
}

// Set gObject to be faceted.
void GObject::setFaceted() {
	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		(*it)->setFaceted();
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		(*it)->setFaceted();
	}
}

// Set gObject to be smooth.
void GObject::setSmooth() {
	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		(*it)->setSmooth();
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		(*it)->setSmooth();
	}
}

	// Set texture to all meshes inside this gObject
void  GObject::setTexture(Texture *tex) {
	Material *mat;
	for(list<TriangleMesh *>::iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		mat = (*it)->getMaterial();
		mat->setTexture(tex);
	}
	for(list<TriangleMesh *>::iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		mat = (*it)->getMaterial();
		mat->setTexture(tex);
	}
}


int  GObject::numTriangles() const {
	int      numPols = 0;
	for(list<TriangleMesh *>::const_iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		numPols += (*it)->numTriangles();
	}
	for(list<TriangleMesh *>::const_iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		numPols += (*it)->numTriangles();
	}
	return numPols;
}

size_t GObject::size() const {
	return m_meshes.size() + m_meshes_transp.size();
}

const TriangleMesh *GObject::at(size_t idx) const {
	const std::list<TriangleMesh *> *ptr = &m_meshes;
	if (idx > m_meshes.size()) {
		ptr = &m_meshes_transp;
		idx -= m_meshes.size();
	}
	std::list<TriangleMesh *>::const_iterator it = ptr->begin();
	std::advance(it, idx);
	return *it;
}


void GObject::print() const {
	printf("*** Opaque meshes:\n");
	for(list<TriangleMesh *>::const_iterator it = m_meshes.begin(), end = m_meshes.end();
		it != end; ++it) {
		(*it)->print();
	}
	printf("*** Transparent meshes:\n");
	for(list<TriangleMesh *>::const_iterator it = m_meshes_transp.begin(), end = m_meshes_transp.end();
		it != end; ++it) {
		(*it)->print();
	}
}
