// -*-C++-*-

#pragma once

#include "texture.h"

// Textures as render target

class TextureRT : public Texture {

public:
	void bind(); // Set this texture as render target
	void unbind(); // Unbind texture and set framebuffer as render target

	friend class TextureManager;

private:
	TextureRT(Texture::type_t type, int height, int width);
	~TextureRT();
	TextureRT(const TextureRT &);
	const TextureRT & operator=(const TextureRT &);

	// FrameBuffer objects
	GLuint m_fbo; // for rendering to texture
	GLuint m_rbo; // render buffer (for depth textures)
	int m_oldViewport[4]; // old viewport coordinates
};
