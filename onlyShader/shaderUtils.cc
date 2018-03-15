#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shaderUtils.h"

// Print error and delete shader

static int CompileError( GLuint shader ) {
	GLint infoLogLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

	GLchar *strInfoLog = new GLchar[ infoLogLength + 1 ];
	glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
	strInfoLog[infoLogLength] = 0;
	fprintf(stderr, "Shader object compilation error :%s\n", strInfoLog);

	free(strInfoLog);
	glDeleteShader(shader);
	return 1;
}

// Print error and delete program

static int CompileLinkError( GLuint program ) {

	GLint infoLogLength;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

	GLchar *strInfoLog = new GLchar[ infoLogLength + 1 ];
	glGetShaderInfoLog(program, infoLogLength, NULL, strInfoLog);
	strInfoLog[infoLogLength] = 0;
	fprintf(stderr, "Shader program compilation error :%s\n", strInfoLog);

	free(strInfoLog);
	glDeleteProgram(program);
	return 1;
}

static int ShaderCompileFailed(GLuint shader) {
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_FALSE) return 0;
	CompileError(shader);
	return 1;
}

static int ProgramLinkFailed(GLuint program) {
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status != GL_FALSE) return 0;
	CompileLinkError(program);
	return 1;
}


GLuint LoadShader(GLenum eShaderType, char *shaderFilename) {

	FILE *fh;
	long int fileSize;
	long int bytesRead;
	char *body;
	int errorCode;
	GLuint shader_obj = 0;

	fh=fopen(shaderFilename,"r");

	if (fh ==NULL) {
		fprintf(stderr, "[E] LoadShader: can't open %s\n",shaderFilename);
		exit(1);
	}
	fseek(fh, 0, SEEK_END);
	fileSize = ftell(fh);
	rewind(fh);
	if (fileSize == 0) {
		errorCode = 1;
		return (GLuint) 0;
	}
	body = new char[fileSize + 1];
	bytesRead = fread(body, 1, fileSize, fh);
	fclose(fh);
	assert(bytesRead <= fileSize);
	body[bytesRead] = 0;
	shader_obj = CompileShader(eShaderType, body, &errorCode);
	free(body);
	if (errorCode) exit(1);
	return shader_obj;
}


GLuint CompileShader(GLenum shaderType, char *shaderText,
					 int *errorCode) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar **) &shaderText, NULL);
	glCompileShader(shader);

	*errorCode = ShaderCompileFailed(shader);
	if (*errorCode) return (GLuint) 0;
	return shader;
}

GLuint CompileShaderFromLines(GLenum shaderType, char **shaderLines, size_t numLines,
							  int *errorCode) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, numLines, (const GLchar **) shaderLines, NULL);
	glCompileShader(shader);
	*errorCode = ShaderCompileFailed(shader);
	if (*errorCode) return (GLuint) 0;
	return shader;
}

GLuint CreateProgramFromObjects(GLuint shaderOne, GLuint shaderTwo, int *errorCode ) {
	GLuint program = glCreateProgram();

	glAttachShader(program, shaderOne);
	glAttachShader(program, shaderTwo);

	// Set attributes

	SetProgramAttribute(program, 0, "v_position");
	SetProgramAttribute(program, 2, "v_normal");
	SetProgramAttribute(program, 8, "v_texCoord");

	glLinkProgram(program);
	*errorCode = ProgramLinkFailed(program);
	if (!*errorCode) {
		glDetachShader(program, shaderOne);
		glDetachShader(program, shaderTwo);
		return program;
	}
	return (GLuint) 0;
}

GLuint CreateProgramFromStrings(char *vertexShader, char *fragmentShader, int *errorCode ) {
	GLuint vertShader;
	GLuint fragShader;
	GLuint program;

	vertShader = CompileShader(GL_VERTEX_SHADER, vertexShader, errorCode);
	if (*errorCode) return (GLuint) 0;
	fragShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader, errorCode);
	if (*errorCode) {
		glDeleteShader(vertShader);
		return (GLuint) 0;
	}
	program = CreateProgramFromObjects(vertShader, fragShader, errorCode);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	if (*errorCode) return (GLuint) 0;
	return program;
}


/////////////////////////////////////////////////////////////////7
// attribute staff

// Set attribute and check for errors

void SetProgramAttribute(GLuint program, int position, char *attribName) {
	GLenum errorCode;
	glBindAttribLocation(program, position, attribName);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "[E] setting attribute \"%s\", position %d, program %d: %s\n", attribName, position, program, gluErrorString(errorCode));
		exit(1);
	}
}


/////////////////////////////////////////////////////////////////7
// uniform staff

GLint GetProgramUniform(char *shaderName, GLuint program, char *unifName) {

	GLint res;
	GLenum errorCode;

	res = glGetUniformLocation(program, unifName);
	if (res == -1) {
		fprintf(stderr, "[W] %s: unfiform %s not found\n", shaderName, unifName);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR) {
			fprintf (stderr, "[W] oprnGL error:: %s\n", gluErrorString(errorCode));
		}
	}
	return res;
}

void shader_set_uniform_1f(GLint unif, GLfloat f0) {
	if (unif != -1) {
		glUniform1f(unif, f0);
	}
}

void shader_set_uniform_2f(GLint unif, GLfloat f0, GLfloat f1) {
	if (unif != -1) {
		glUniform2f(unif, f0, f1);
	}
}

void shader_set_uniform_3f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2) {
	if (unif != -1) {
		glUniform3f(unif, f0, f1, f2);
	}
}

void shader_set_uniform_4f(GLint unif, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3) {
	if (unif != -1) {
		glUniform4f(unif, f0, f1, f2, f3);
	}
}

void shader_set_uniform_1i(GLint unif, int f0) {
	if (unif != -1) {
		glUniform1i(unif, f0);
	}
}

void shader_set_uniform_2i(GLint unif, int f0, int f1) {
	if (unif != -1) {
		glUniform2i(unif, f0, f1);
	}
}

void shader_set_uniform_3i(GLint unif, int f0, int f1, int f2) {
	if (unif != -1) {
		glUniform3i(unif, f0, f1, f2);
	}
}

void shader_set_uniform_4i(GLint unif, int f0, int f1, int f2, int f3) {
	if (unif != -1) {
		glUniform4i(unif, f0, f1, f2, f3);
	}
}

void shader_set_uniform_4fv(GLint unif, GLfloat *ptr) {
	if (unif != -1) {
		glUniform4fv(unif, 1, ptr);
	}
}

void shader_set_uniform_matrix4(GLint unif, GLfloat *ptr) {
	if (unif != -1) {
		glUniformMatrix4fv(unif, 1, GL_FALSE, ptr);
	}
}
