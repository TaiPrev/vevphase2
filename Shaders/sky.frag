#version 120

varying vec3 f_texCoord;
uniform samplerCube cubemap;

void main() {
	gl_FragColor = textureCube(cubemap, f_texCoord);
}
