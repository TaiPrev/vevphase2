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
	//S.C. Camara
	vec4 P4 = modelToCameraMatrix * vec4(v_position, 1.0);	//positionEye
	N = modelToCameraMatrix * vec4(v_normal, 0.0);
	T = modelToCameraMatrix * vec4(v_TBN_t, 0.0);
	B = modelToCameraMatrix * vec4(v_TBN_b, 0.0);
	V = (0.0, 0.0, 0.0, 1.0) - P4;					//V of view

	//construir matriz de CameraToTangent
	mat3 base_cameraToTanget = mat3(T.xyz, B.xyz, N.xyz);
	base_cameraToTanget = transpose(base_cameraToTanget);
	cameraToTangentMatrix = mat4(base_cameraToTanget);	//construye la matriz 4x4 a partir de la 3x3 transpuesta previamente obtenida

	//pasar V y positionEye al espacio tangente
	vec4 V4 = cameraToTangentMatrix * V;
	f_viewDirection = V4.xyz;
	for (int i = 0; i<4; i++){
		if(theLights[i].position.w == 0.0) {
		  // direction light   vec3 L = normalize(-theLights[i].position.xyz);
		  vec4 aux = cameraToTangentMatrix * normalize(-theLights[i].position);
		  f_lightDirection[i] = aux.xyz;
		}
		else{
			//position or spot ligh  L = normalize(theLights[i].position.xyz - positionEye);
		  vec4 aux = cameraToTangentMatrix * normalize(theLights[i].position - P4);
		  f_lightDirection[i] = aux.xyz;
		}
		
		vec4 aux = cameraToTangentMatrix * vec4(theLights[i].spotDir, 0.0);
		f_spotDirection[i] = aux.xyz;
	}

	//mat3 MV3x3 = mat3(modelToCameraMatrix); // 3x3 modelview matrix

	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}
