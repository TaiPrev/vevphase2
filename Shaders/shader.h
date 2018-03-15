// -*-C++-*-

#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

class ShaderProgram {

public:

	void activate();
	void deactivate();
	/**
	 * Set-up shader uniforms with state information. Call it before drawing
	 * anything.
	 */
	void beforeDraw();

	const std::string &getName() const;
	friend class ShaderManager;

private:

	/**
	 * Create a Shader Program
	 *
	 * @param shaderName name of the shader program.
	 * @param vShaderFile filename of the vertex shader.
	 * @param fShaderFile filename of the fragment shader
	 *
	 * @return the newly created shader program
	 */
	ShaderProgram(const std::string & name,
				  const std::string & vShaderFile,
				  const std::string & fShaderFile);

	~ShaderProgram();
	ShaderProgram(const ShaderProgram &);
	ShaderProgram & operator=(const ShaderProgram &);

	void initUniforms();

	struct  LightUniform {
		GLint position;
		GLint diffuse;
		GLint specular;
		GLint attenuation;
		GLint spotDir;     // only for spotLights
		GLint cosCutOff;   // only for spotLights
		GLint exponent;    // only for spotLights
	};

	struct  MaterialUniform {
		GLint diffuse;
		GLint specular;
		GLint alpha;
		GLint shininess;
	};

	std::string m_name;

	GLuint m_vShader, m_fShader, m_program;
	GLuint m_oldProgram; // active program when activate was called

	// uniform handlers
	GLint m_umodeltoCamera;
	GLint m_umodeltoWorld;
	GLint m_ucameraToClip;
	GLint m_umodeltoClip;
	GLint m_uactiveLights;
	GLint m_uambient;
	LightUniform m_ulight[4];     // MG_MAX_LIGHTS
	MaterialUniform m_umat;
	GLint m_utexSampler;
	GLint m_ubumpSampler;
	GLint m_utexCubemap;

};
