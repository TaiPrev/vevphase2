#version 120

// Displacement Vertex with many lights

//FIRST IMPLEMENTATION, 1 LIGHT

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space, used as direction of ray to be traced by texture space
varying vec3 f_lightDirection; // tangent space
varying vec3 f_spotDirection;  // tangent space
varying vec3 f_normal;	//normal pasada al espacio de la tangente, añadido por mi

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
	cameraToTangentMatrix = mat4(base_cameraToTanget);	

	//gems2-ch08: Vertex shader 
	/*
		The vertex shader for distance mapping, is remarkably similar to
		a vertex shader for tangent-space normal mapping, with two notable differences. The
		first is that in addition to transforming the light vector into tangent space, we also
		transform the viewing direction into tangent space. This tangent-space eye vector is
		used in the vertex shader as the direction of the ray to be traced in texture space. The
		second difference is that we incorporate an additional factor inversely proportional to
		the perceived depth. This allows us to adjust the scale of the displacements interactively.
	*/

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
