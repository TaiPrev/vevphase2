#version 120

uniform mat4 modelToCameraMatrix; // M
uniform mat4 modelToClipMatrix;
uniform mat4 cameraToClipMatrix;  // P

uniform float time;

attribute vec3 v_position;

varying vec4 f_color;

void main() {
	
	if(time<1){f_color = vec4(time,(1-time),(1-time),1);}
	else{f_color = vec4(0,0,0,1);}
	gl_Position = modelToClipMatrix * vec4(v_position, 1);
}
