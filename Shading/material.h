// -*-C++-*-

#pragma once

#include <string>
#include "vector3.h"
#include "texture.h"

class Material {

public:

	// get
	float getAlpha() const;
	const Vector3 &getDiffuse() const;
	const Vector3 &getSpecular() const;
	float getShininess() const;
	Texture *getTexture () const;
	Texture *getBumpMap () const;
	// set
	void setAlpha(float alpha);
	void setDiffuse(const Vector3 &rgb);
	void setSpecular(const Vector3 &rgb, float shininess);
	void setTexture (Texture * theTexture);
	void setBumpMap (Texture * bumpMapexture);
	Texture *getTexture();
	Texture *getBumpMap();
	// query
	bool isTransp() const;
	bool hasTexture() const;
	bool hasBump() const;
	// print
	void print() const;

	friend class MaterialManager;
private:
	Material(const std::string & lib, const std::string & name);
	~Material();
	Material();
	Material(const Material &);
	Material & operator =(const Material &);

	// The default values that we use
	// are those defined by OpenGL
	std::string m_library;
	std::string m_name;

	Vector3  m_diffuse;          // GL_DIFFUSE  : RGB default (0.8, 0.8, 0.8)
	int      m_hasSpecular;
	Vector3  m_specular;         // GL_SPECULAR : RGB default (0.0, 0.0, 0.0)
	float    m_shininess;        // GL_SHININESS: default 0.0 ; range [0,128]
	float    m_alpha;            // alpha value. Default 1
	Texture *m_tex;              // Texture
	Texture *m_bump;             // Bump-mapping texture
};
