#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>
#include "texturert.h"

using std::string;
using std::map;

TextureRT::TextureRT(Texture::type_t mode, int h, int w) :
	m_fbo(0),
	m_rbo(0) {
	// set Render Target (RT) texture:
	//  - properly set filters and anti-aliasing setings
	//  - reclaim memory in OpenGL for storing the texture
	m_type = mode;
	m_oldViewport[0] = 0;
	m_oldViewport[1] = 0;
	m_oldViewport[2] = 0;
	m_oldViewport[3] = 0;
	setMipmap(false);
	m_height = h;
	m_width = w;
	if (m_type == Texture::rt_depth) {
		m_components = 1;
		m_format = GL_DEPTH_COMPONENT;
	} else {
		m_components = 3;
		m_format = GL_RGB;
	}
	m_size = m_components * m_height * m_width;
	if (!m_size) {
		fprintf(stderr, "[E] texture with zero size\n");
		exit(1);
	}
	setFilters(GL_NEAREST, GL_NEAREST);
	setWrapST(GL_CLAMP, GL_CLAMP);
	// Reclaim OpenGL space for image data
	Texture::bindGL();
	glTexImage2D(m_target, 0, m_format, m_width,
				 m_height, 0, m_format, GL_UNSIGNED_BYTE,
				 0);
	Texture::unbindGL();
	// Set texture fbo
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	if (m_type == Texture::rt_depth) {
		// Instruct openGL that we won't bind a color texture with the currently binded FBO
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		// attach depth texture to current fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_id, 0);
	} else {
		// Color maps also need depth buffer handling
		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		// attach renderbuffer object to the current fbo
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		// attach color texture to current fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);
	}
	//Does the GPU support current FBO configuration?
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "[E] %s\n", gluErrorString(err));
		exit(1);
	}
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		Texture::print();
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			fprintf(stderr, "[E] Problem with FBOs: attachment problem: 0x%x\n", status);
			exit(1);
		}
		fprintf(stderr, "[E] This GPU does not support FBOs\n");
		exit(1);
	}
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureRT::~TextureRT() {
	if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
	if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);
}

// Set this texture as render target
void TextureRT::bind() {
	// Save old viewport
	glGetIntegerv(GL_VIEWPORT, &(m_oldViewport[0]));
	//set the viewport to be the size of the texture
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	if(m_type != Texture::rt_depth) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	}
	glViewport(0,0, m_width, m_height);
}

void TextureRT::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(m_type != Texture::rt_depth) {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	} else {
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	// Set original viewport
	glViewport(m_oldViewport[0],m_oldViewport[1],
			   m_oldViewport[2], m_oldViewport[3]);
}
