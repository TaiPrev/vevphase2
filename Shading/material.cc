#include <cstdio>
#include <string>
#include <assert.h>
#include "tools.h"
#include "material.h"
#include "textureManager.h"

using std::string;

Material::Material(const std::string & lib, const std::string & name) :
	m_library(lib),
	m_name(name),
	m_diffuse(Vector3(0.8f, 0.8f, 0.8f)),
	m_hasSpecular(false),
	m_specular(Vector3::ZERO),
	m_shininess(65.0f),
	m_alpha(1.0f),
	m_tex(TextureManager::instance()->whiteTexture()),
	m_bump(0) {}

Material::~Material() {};

// get
float  Material::getAlpha() const { return m_alpha; }
const Vector3 &Material::getDiffuse() const { return m_diffuse; }
const Vector3 &Material::getSpecular() const { return m_specular; }
float  Material::getShininess() const { return m_shininess; }
Texture *Material::getTexture() { return m_tex; }
Texture *Material::getBumpMap() { return m_bump; }

//Texture *Material::getTextureMaterial() const { return m_tex; }
// Texture *Material::getBumpMap() const { return m_bump; }

// set
void Material::setAlpha(float alpha)  { m_alpha = alpha; }
void Material::setDiffuse(const Vector3 &rgb) { m_diffuse = rgb; }
void Material::setSpecular(const Vector3 &rgb, float shininess) {
	m_hasSpecular = true;
	m_shininess = shininess;
	m_specular = rgb;
}
void Material::setTexture (Texture * tex) {	m_tex = tex; }
void Material::setBumpMap (Texture * bump) { m_bump = bump; }

// query
bool Material::isTransp() const {
	return m_alpha != 1.0;
}
bool Material::hasTexture() const { return m_tex != 0; }
bool Material::hasBump() const { return m_bump != 0; }

// print
void Material::print() const {
	printf("Library:%s Name: %s\n", m_library.c_str(), m_name.c_str() );
	printf(" Diffuse  %0.4f %0.4f %0.4f\n",
		   m_diffuse[0], m_diffuse[1],
		   m_diffuse[2]);
	printf(" Transparent: ");
	if ( isTransp() ) {
		printf("yes (%0.4f)\n", m_alpha);
	} else {
		printf("no\n");
	}
	printf(" Specular %0.4f %0.4f %0.4f\n",
		   m_specular[0], m_specular[1],
		   m_specular[2]);
	printf(" Shininess %5.1f\n", m_shininess );
	if (m_tex) {
		printf("Texture: ");
		m_tex->print();
	}
	if (m_bump) {
		printf("Bump map: ");
		m_bump->print();
	}
}
