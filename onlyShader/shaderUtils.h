// -*-C++-*-
#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GL/glew.h>
#include <GL/glut.h>

// @@ Documentation

GLuint LoadShader(GLenum eShaderType, char *shaderFilename);

GLuint CompileShader(GLenum shaderType, char *shaderText, int *errorCode);

GLuint CompileShaderFromLines(GLenum shaderType, char **shaderLines, size_t numLines,
							  int *errorcode);

GLuint CreateProgramFromObjects(GLuint shaderOne, GLuint shaderTwo, int *errorCode );

GLuint CreateProgramFromStrings(char *vertexShader, char *fragmentShader, int *errorCode );

void SetProgramAttribute(GLuint program, int position, char *attribName);

GLint GetProgramUniform(char *shaderName, GLuint program, char *unifName);

void shader_set_uniform_1f(GLint unif, GLfloat f0);
void shader_set_uniform_2f(GLint unif, GLfloat f0, GLfloat f1);
void shader_set_uniform_3f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2);
void shader_set_uniform_4f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);
void shader_set_uniform_1i(GLint unif, int f0);
void shader_set_uniform_2i(GLint unif, int f0, int f1);
void shader_set_uniform_3i(GLint unif, int f0, int f1, int f2);
void shader_set_uniform_4i(GLint unif, int f0, int f1, int f2, int f3);
void shader_set_uniform_4fv(GLint unif, GLfloat *ptr);
void shader_set_uniform_matrix4(GLint unif, GLfloat *ptr);

#endif
