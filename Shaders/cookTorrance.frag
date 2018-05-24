#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

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

uniform sampler2D texture0;

//they all DON'T COME NORMALIZED, BUT SHAN'T BE SO IN THEESE VARIABLES
varying vec3 f_position;      // camera space
varying vec3 f_viewDirection; // camera space
varying vec3 f_normal;        // camera space
varying vec2 f_texCoord;

const float pi = 3.14159265358979323846264;
// r = roughness = rugosidad , rango = [ 0 , 1 ]
const float roughness;
// lambda = factor de reflectancia de Fresnel ,
// rango = [ 0.01 , 0.95 ]
const float lambda;

//aproximación de Schlick
const float fresnel (const vec3 L,
					 const vec3 H){
	return lambda + (1-lambda)*pow( (1-dot(L, H)) ,5.0)
}

const float termino_geometrico (const vec3 L,
						 const vec3 V,
						 const vec3 H,
						 const vec3 N){
	float masking = (2*dot(N,H)*dot(N,V))/dot(V,H);
	float shadowing = (2*dot(N,H)*dot(N,L))/dot(V,H);
	float aux = min(masking, shadowing);
	return min(1.0, aux);
}

const float beckman (const vec3 H,
				const vec3 N){
	float exp = (pow( dot(H,N), 2.0)-1.0)/(pow(roughness,2.0) * pow(dot(H,N), 2.0));
	float D = 1 / (pi * pow(roughness,2.0) * pow(dot(H,N), 4.0));
	return pow(D, exp);
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


void spot_light(const in int i,
				const in vec3 position,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 diffuse, inout vec3 specular) {

}

void main(){
	//acumuladores
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	//NORMALIZACIONES DE LOS VARYING
	vec3 P = f_position;
	vec3 V = normalize(f_viewDirection);
	vec3 N = normalize(f_normal);

	for(int i=0; i<active_lights_n; i++){
		if(theLights[i].position.w == 0.0) {
		  // direction light
		  vec3 L = normalize(-theLights[i].position.xyz);
		  direction_light(i, L, V, N, diffuse, specular);
		} 
		  else {
		  if (theLights[i].cosCutOff == 0.0) {
			// point light
			point_light(i, P, V, N, diffuse, specular);
		  } 
		  else {
			// spot light
			spot_light(i, P, V, N, diffuse, specular);
		  }
		}
	}
	vec4 f_color = vec4(diffuse+specular, 1.0);
	gl_FragColor = f_color * texture2D(texture0, f_texCoord);
}