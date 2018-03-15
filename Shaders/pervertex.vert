#version 120

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 modelToClipMatrix;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient;  // rgb

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

uniform struct material_t {
	vec3  diffuse;
	vec3  specular;
	float alpha;
	float shininess;
} theMaterial;

attribute vec3 v_position; // Model space
attribute vec3 v_normal;   // Model space
attribute vec2 v_texCoord;

varying vec4 f_color;
varying vec2 f_texCoord;

float lambert(vec3 n, const vec3 l) {
	return 1.0;
}

float specular_channel(const vec3 n,
					   const vec3 l,
					   const vec3 v,
					   float m) {
	return 1.0;
}

void direction_light(const in int i,
					 const in vec3 lightDirection,
					 const in vec3 viewDirection,
					 const in vec3 normal,
					 inout vec3 diffuse, inout vec3 specular) {
}

void point_light(const in int i,
				 const in vec3 position,
				 const in vec3 viewDirection,
				 const in vec3 normal,
				 inout vec3 diffuse, inout vec3 specular) {
}


// Note: no attenuation in spotlights

void spot_light(const in int i,
				const in vec3 lightDirection,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 diffuse, inout vec3 specular) {
}

void main() {

	// for(int i=0; i < active_lights_n; ++i) {
	//	if(theLights[i].position.w == 0.0) {
	//	  // direction light
	//	} else {
	//	  if (theLights[i].cosCutOff == 0.0) {
	//		// point light
	//	  } else {
	//		// spot light
	//	  }
	//	}
	// }

	f_color = vec4(1.0);
	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}
