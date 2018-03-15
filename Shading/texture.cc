#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include "texture.h"
#include "tools.h"
#include "imageManager.h"

using std::string;

static const char *TT_string(Texture::type_t e);
static const char *PT_string(GLenum e);

Texture::Texture() :
	m_type(empty),
	m_name("<noname>"),
	m_target(GL_TEXTURE_2D),
	m_id(0),
	m_size(0),
	m_height(0),
	m_width(0),
	m_components(3),
	m_format(GL_RGB),
	m_mipmap(true),
	m_magFilter(GL_LINEAR),
	m_minFilter(GL_LINEAR),
	m_wrapS(GL_REPEAT),
	m_wrapT(GL_REPEAT),
	m_img(0) {
	//Allocates a texture name
	glGenTextures( 1, &m_id );
}

Texture::~Texture() {
	if (m_id) glDeleteTextures(1, &m_id);
}

Texture::Texture(const std::string & name) :
	m_type(empty),
	m_name(name),
	m_target(GL_TEXTURE_2D),
	m_id(0),
	m_size(0),
	m_height(0),
	m_width(0),
	m_components(3),
	m_format(GL_RGB),
	m_mipmap(true),
	m_magFilter(GL_LINEAR),
	m_minFilter(GL_LINEAR),
	m_wrapS(GL_REPEAT),
	m_wrapT(GL_REPEAT),
	m_img(0) {
	glGenTextures( 1, &m_id );
}

Texture::type_t Texture::getType() const { return m_type; }

void Texture::setImage(const string &FileName) {

	if(m_img) {
		//remove image data from openGL
		glDeleteTextures(1, &m_id);
		//Allocates a texture name
		glGenTextures(1, &m_id);
	}

	m_type = tex;
	m_img = ImageManager::instance()->create(FileName);
	m_size = m_img->getSize();
	m_height = m_img->getHeight();
	m_width = m_img->getWidth();
	m_format = GL_RGB;
	m_components = 3;
	// Filters
	if (m_mipmap) {
		// Filters
		m_magFilter = GL_LINEAR;
		m_minFilter = GL_NEAREST_MIPMAP_NEAREST;
	} else {
		m_magFilter = GL_LINEAR;
		m_minFilter = GL_LINEAR;
	}
	// Send image into openGL texture
	sendImageGL();
	// Send texture data to OpenGL (initialize OpenGL texture)
	sendParamsGL();
}

void Texture::setBumpMap(const string &FileName) {

	setImage(FileName);
	m_mipmap = 0; // no mipmap for bump textures
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	m_type = bumpmap;
	sendParamsGL();
}

void Texture::setProj(const string &FileName) {

	setImage(FileName);
	m_mipmap = 0; // no mipmap for projective textures
	m_type = proj;
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	m_wrapS = GL_CLAMP_TO_BORDER;
	m_wrapT = GL_CLAMP_TO_BORDER;
	sendParamsGL();
}

void Texture::setCubeMap(const string &xpos, const string &xneg,
						 const string &ypos, const string &yneg,
						 const string &zpos, const string &zneg) {

	m_type = cubemap;
	m_target = GL_TEXTURE_CUBE_MAP;

	Image *img;
	int i = 0;
	GLuint targets[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	const string *names[6] = { &xpos, &xneg, &ypos, &yneg, &zpos, &zneg };


	/* // Typical cube map settings */
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	m_wrapS = GL_CLAMP_TO_EDGE;
	m_wrapT = GL_CLAMP_TO_EDGE;
	m_wrapR = GL_CLAMP_TO_EDGE;
	ImageManager *imgr = ImageManager::instance();
	bindGL();
	for(i = 0; i < 6; i++) {
		img = imgr->create(*names[i]);
		glTexImage2D(targets[i], 0, 3,
					 img->getWidth(), img->getHeight(),
					 0, GL_RGB, GL_UNSIGNED_BYTE, img->getData());
	}
	sendParamsGL();
	unbindGL();
}

void Texture::setWhiteTexture() {

	static unsigned char white[3] = {255, 255, 255};

	if(m_img) {
		//remove image data from openGL
		glDeleteTextures(1, &m_id);
		//Allocates a texture name
		glGenTextures( 1, &m_id );
	}
	m_size = 3;
	m_height = 1;
	m_width = 1;
	m_format = GL_RGB;
	m_components = 3;
	m_mipmap = 0;
	m_magFilter = GL_NEAREST;
	m_minFilter = GL_NEAREST;
	m_wrapS = GL_REPEAT;
	m_wrapT = GL_REPEAT;
	// Set our texture as active
	bindGL();
	glTexImage2D( m_target, 0, m_components, m_width,
				  m_height, 0, m_format, GL_UNSIGNED_BYTE,
				  &white[0]);
	// Send texture data to OpenGL (initialize OpenGL texture)
	sendParamsGL();
	unbindGL();
}

/* Set/get functions */

bool Texture::setMipmap(bool mipmap) {
	bool old = m_mipmap;
	m_mipmap = mipmap;
	return old;
}
bool Texture::getMipmap() const { return m_mipmap; }


// Wrap

void Texture::setWrapST(GLenum wrapS, GLenum wrapT) {
	m_wrapS = wrapS;
	m_wrapT = wrapT;
	sendParamsGL();
}

void Texture::setWrapS(GLenum wrapS) {
	m_wrapS = wrapS;
	sendParamsGL();
}

void Texture::setWrapT(GLenum wrapT){
	m_wrapT = wrapT;
	sendParamsGL();
}
// Filters

void Texture::setMinFilter(GLenum minFilter) {
	m_minFilter = minFilter;
	sendParamsGL();
}

void Texture::setMagFilter(GLenum magFilter) {
	m_magFilter = magFilter;
	sendParamsGL();
}

void Texture::setFilters(GLenum minFilter, GLenum magFilter) {
	m_magFilter = magFilter;
	m_minFilter = minFilter;
	sendParamsGL();
}


// BindGLTexture
//
// Set texture as "active" in the zero texture unit. Subsequent texture
// operations are done on this texture.

void Texture::bindGL() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( m_target, m_id );
}

void Texture::unbindGL() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( m_target, 0 );
}


void Texture::bindGLUnit(int location) {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( m_target, m_id );
}

void Texture::unbindGLUnit(int location) {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( m_target, 0 );
}

static int cycleTexEnum(GLenum *fil, int m,
						GLenum theFil) {
	int fil_i = 0;
	for(fil_i = 0; fil_i != m; ++fil_i) {
		if(fil[fil_i] == theFil)
			break;
	}
	fil_i++;
	return (fil_i % m);
}

void Texture::cycleWrapS() {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	int idx = cycleTexEnum(wrap_fil, 2, m_wrapS);
	setWrapS(wrap_fil[idx]);
}

void Texture::cycleWrapT() {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	int idx = cycleTexEnum(wrap_fil, 2, m_wrapT);
	setWrapT(wrap_fil[idx]);
}

void Texture::cycleMagFilter() {

	static GLenum mag_fil[] = { GL_NEAREST, GL_LINEAR };
	int idx = cycleTexEnum(mag_fil, 2, m_magFilter);
	setMagFilter(mag_fil[idx]);
}

void Texture::cycleMinFilter() {
	static GLenum min_fil[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
								GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
								GL_LINEAR_MIPMAP_LINEAR };
	int idx = cycleTexEnum(min_fil, 6, m_minFilter);
	setMinFilter(min_fil[idx]);
	//printf("%s\n", min_fil_str[idx]);
}

/**
 * Send texture parameters to OpenGL
 *
 * hints:
 *        - use glTexParameteri for setting filters/wrap
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */

void Texture::sendParamsGL() {

	// Set our texture as active
	bindGL();
	// Set OpenGL texture wrap
	glTexParameteri( m_target, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri( m_target, GL_TEXTURE_WRAP_T, m_wrapT);
	if (m_type == cubemap) {
		glTexParameterf(m_target, GL_TEXTURE_WRAP_R,
						m_wrapR);
	}
	// Set OpenGL texture filters
	glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, m_magFilter);
	glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
	unbindGL();
}

/**
 * Send image to OpenGL
 *
 * hints:
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */

void Texture::sendImageGL() {

	if (!m_img) return;

	// Set our texture as active
	bindGL();
	if (m_img->getHeight() && m_img->getWidth()) {
		// Load image to OpenGL texture
		if (m_mipmap) {
			gluBuild2DMipmaps(m_target, m_components, m_img->getWidth(), m_img->getHeight(),
							  m_format, GL_UNSIGNED_BYTE, m_img->getData());
		} else {
			glTexImage2D( m_target, 0, m_components, m_img->getWidth(),
						  m_img->getHeight(), 0, m_format, GL_UNSIGNED_BYTE,
						  m_img->getData());
		}
	}
	unbindGL();
}

static const char *TT_string(Texture::type_t e) {

	static Texture::type_t N[] = {Texture::empty, Texture::tex, Texture::rt_depth, Texture::rt_color, Texture::cubemap, Texture::bumpmap, Texture::proj};
	static const char *T[]  = {"empty", "tex", "rt_depth", "rt_color", "cubemap", "bumpmap", "proj"};

	int i;
	int m;

	m = sizeof(T) / sizeof(char *);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}


static const char *PT_string(GLenum e) {

	static GLenum N[] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_RGB, GL_RGB8, GL_DEPTH_COMPONENT};
	static const char * T[]  = {"GL_TEXTURE_2D", "GL_TEXTURE_CUBE_MAP", "GL_CLAMP", "GL_CLAMP_TO_BORDER", "GL_REPEAT", "GL_LINEAR", "GL_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", "GL_NEAREST_MIPMAP_NEAREST", "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_NEAREST", "GL_RGB", "GL_RGB8", "GL_DEPTH_COMPONENT"};
	int i;
	int m;

	m = sizeof(T) / sizeof(const string &);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}

void Texture::print() const {

	printf("Texture id:%d\tName: %s\tType: %s\ttarget:%s\n", m_id, m_name.c_str(), TT_string(m_type), PT_string(m_target));
	if (!m_img) {
		printf ("No jpeg image\n");
	} else {
		printf("JPG image: %s\tResolution: %d x %d\tSize: %lu\tMipmap: %d\tData: %p\n", m_img->getName().c_str(), m_img->getWidth(), m_img->getHeight(),
			   m_size, m_mipmap, m_img->getData());

	}
	if (!m_img && m_size) {
		printf("Generated image\tResolution: %d x %d \t Size: %lu\tMipmap: %d\n", m_width, m_height,
			   m_size, m_mipmap);
	}
	printf("Format: %s\tComponents:%d\n", PT_string(m_format), m_components);
	printf("Filters (min,mag): %s, %s\t Wrap (s,t): %s,%s\n\n",
			 PT_string(m_minFilter), PT_string(m_magFilter),
			 PT_string(m_wrapS), PT_string(m_wrapT));
}
