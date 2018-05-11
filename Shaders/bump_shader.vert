#version 120

// Bump mapping with many lights.
//
// All computations are performed in the tangent space; therefore, we need to
// convert all light (and spot) directions and view directions to tangent space
// and pass them the fragment shader.

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space

// all attributes in model space
attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_texCoord;
attribute vec3 v_TBN_t;
attribute vec3 v_TBN_b;

uniform mat4 modelToCameraMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToClipMatrix;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

void main() {
	vec3 L[4];	//the 4 lights
	mat4 cameraToTangentMatrix;
	vec4 N; vec4 T; vec4 B; vec4 V;	//Normal, Tanget, Bitangent and V vector for SCCamera

	vec4 P4 = modelToCameraMatrix * vec4(v_position, 1.0);	//positionEye

	N = modelToCameraMatrix * vec4(v_normal, 0.0);
	T = modelToCameraMatrix * vec4(v_TBN_t, 0.0);
	B = modelToCameraMatrix * vec4(v_TBN_b, 0.0);
	V = (0.0, 0.0, 0.0, 1.0) - P4;					//V



	//mat3 MV3x3 = mat3(modelToCameraMatrix); // 3x3 modelview matrix

	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
}
