#include <cstdio>
#include <cmath>
#include "constants.h"
#include "tools.h"
#include "shader.h"
#include "shaderUtils.h"
#include "lightManager.h"
#include "texture.h"
#include "material.h"
#include "renderState.h"

void ShaderProgram::initUniforms() {

	static char buffer[1024];
	int i;
	const char *name = m_name.c_str();
	// Fill uniforms for all possible lights
	for (i = 0; i < 4; ++i) { // MG_MAX_LIGHTS
		sprintf(buffer, "theLights[%d].position", i);
		m_ulight[i].position = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].diffuse", i);
		m_ulight[i].diffuse = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].specular", i);
		m_ulight[i].specular = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].attenuation", i);
		m_ulight[i].attenuation = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].spotDir", i);
		m_ulight[i].spotDir = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].cosCutOff", i);
		m_ulight[i].cosCutOff = GetProgramUniform(name, m_program, buffer);
		sprintf(buffer, "theLights[%d].exponent", i);
		m_ulight[i].exponent = GetProgramUniform(name, m_program, buffer);
	}
	m_uactiveLights = GetProgramUniform(name, m_program, "active_lights_n");
	m_uambient = GetProgramUniform(name, m_program, "scene_ambient");

	m_umat.diffuse = GetProgramUniform(name, m_program, "theMaterial.diffuse");
	m_umat.specular = GetProgramUniform(name, m_program, "theMaterial.specular");
	m_umat.shininess = GetProgramUniform(name, m_program, "theMaterial.shininess");
	m_umat.alpha = GetProgramUniform(name, m_program, "theMaterial.alpha");

	m_utexSampler = GetProgramUniform(name, m_program, "texture0");
	m_ubumpSampler = GetProgramUniform(name, m_program, "bumpmap");
	m_utexCubemap = GetProgramUniform(name, m_program, "cubemap");

	m_umodeltoCamera = GetProgramUniform(name, m_program, "modelToCameraMatrix");
	m_umodeltoWorld = GetProgramUniform(name, m_program, "modelToWorldMatrix");
	m_ucameraToClip = GetProgramUniform(name, m_program, "cameraToClipMatrix");
	m_umodeltoClip = GetProgramUniform(name, m_program, "modelToClipMatrix");

}

ShaderProgram::~ShaderProgram() {}

ShaderProgram::ShaderProgram(const std::string & shaderName,
							 const std::string & vShaderFile,
							 const std::string & fShaderFile) :
	m_name(shaderName),
	m_vShader(0),
	m_fShader(0),
	m_program(0) {

	fprintf(stderr, "[I] Shader %s\n", shaderName.c_str());
	fprintf(stderr, "[I] compiling %s ...", vShaderFile.c_str());
	m_vShader = LoadShader(GL_VERTEX_SHADER, vShaderFile.c_str());
	fprintf(stderr, "OK\n");
	fprintf(stderr, "[I] compiling %s ...", fShaderFile.c_str());
	m_fShader = LoadShader(GL_FRAGMENT_SHADER, fShaderFile.c_str());
	fprintf(stderr, "OK\n");
	fprintf(stderr, "[I] linking ...");
	m_program = CreateProgramFromObjects(shaderName.c_str(),
										 m_vShader,
										 m_fShader);
	fprintf(stderr, "OK (program: %d)\n", m_program);
	initUniforms();
}

const std::string &ShaderProgram::getName() const { return m_name; }

void ShaderProgram::activate() {

	GLenum errorCode;

	glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &(m_oldProgram));
	glUseProgram(m_program);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "[E] ShaderProgram::activate program %d: %s\n", m_program, gluErrorString(errorCode));
		exit(1);
	}
}

void ShaderProgram::deactivate() {
	GLenum errorCode;

	glUseProgram(m_oldProgram);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "[E] ShaderProgram::deactivate program %d: %s\n", m_program, gluErrorString(errorCode));
		exit(1);
	}
}

void ShaderProgram::beforeDraw() {

	Material *mat;
	Texture *tex;
	RenderState *rs = RenderState::instance();

	shader_set_uniform_matrix4(m_umodeltoCamera, rs->getGLMatrix(RenderState::modelview));
	shader_set_uniform_matrix4(m_umodeltoWorld, rs->getGLMatrix(RenderState::model));
	shader_set_uniform_matrix4(m_ucameraToClip, rs->getGLMatrix(RenderState::projection));
	shader_set_uniform_matrix4(m_umodeltoClip, rs->getGLMatrix(RenderState::modelview_projection));
	shader_set_uniform(m_uambient, rs->getSceneAmbient());

	int i = 0;
	for(LightManager::iterator it = LightManager::instance()->begin(), end = LightManager::instance()->end();
		it != end; ++it) {
		Light *theLight = *it;
		if (!theLight->isOn()) continue;
		if (i == 4) { // MG_MAX_LIGHTS
			fprintf(stderr, "[W] too many active lights. Discarding the rest\n");
			break;
		}
		shader_set_uniform_4fv(m_ulight[i].position, theLight->getPositionEye_4fv());
		shader_set_uniform(m_ulight[i].diffuse, theLight->getDiffuse());
		shader_set_uniform(m_ulight[i].specular, theLight->getSpecular());
		shader_set_uniform(m_ulight[i].attenuation, theLight->getAttenuationVector());
		if (theLight->isSpot()) {
			shader_set_uniform(m_ulight[i].spotDir, theLight->getSpotDirectionEye());
			shader_set_uniform_1f(m_ulight[i].exponent, theLight->getSpotExponent());
			shader_set_uniform_1f(m_ulight[i].cosCutOff, cosf(theLight->getSpotCutoff()));
		} else {
			shader_set_uniform_1f(m_ulight[i].cosCutOff, 0.0f); // if (cos) cutoff is zero -> no spotLight
		}
		++i;
	}
	shader_set_uniform_1i(m_uactiveLights, i);

	mat = rs->getFrontMaterial();
	if (mat != 0) {
		shader_set_uniform(m_umat.diffuse, mat->getDiffuse());
		shader_set_uniform(m_umat.specular, mat->getSpecular());
		shader_set_uniform_1f(m_umat.shininess, mat->getShininess());
		shader_set_uniform_1f(m_umat.alpha, mat->getAlpha());
		tex = mat->getTexture();
		if (tex != 0) {
			// Set texture to unit 0
			tex->bindGLUnit(Constants::gl_texunits::texture);
			shader_set_uniform_1i(m_utexSampler, Constants::gl_texunits::texture); // Texture unit 0
		}
		tex = mat->getBumpMap();
		if (tex != 0) {
			// bumpMapping in texture unit 1
			tex->bindGLUnit(Constants::gl_texunits::bump);
			shader_set_uniform_1i(m_ubumpSampler, Constants::gl_texunits::bump); // Bump Texture unit 1
		}
	}
}
