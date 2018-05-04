#version 120

attribute vec3 v_position;	//s.c. modelo
attribute vec3 v_normal;	//s.c. camara
attribute vec2 v_texCoord;	

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 modelToClipMatrix;

//variables to be calculated HERE and then be passed on to .frag
varying vec3 f_position;
varying vec3 f_viewDirection;
varying vec3 f_normal;
varying vec2 f_texCoord;

void main() {

	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
	vec4 P4 = modelToCameraMatrix * vec4(v_position, 1.0);	//positionEye
	vec4 N4 = modelToCameraMatrix * vec4(v_normal, 0.0);	//normal
	vec4 V4 = (0.0, 0.0, 0.0, 1.0) - P4;					//V

	f_position = P4.xyz;
	f_normal = N4.xyz;
	f_viewDirection =  V4.xyz;
	f_texCoord = v_texCoord;
}
