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
const float roughness = 0.1;
// lambda = factor de reflectancia de Fresnel ,
// rango = [ 0.01 , 0.95 ]
const float lambda = 0.7;
const float kd = 0.6;


float lambert(const vec3 n, const vec3 l) {
	return max(0.0, dot(n,l));
}

//aproximación de Schlick
 float fresnel (const vec3 L,
					 const vec3 H){
	return lambda + (1-lambda)*pow( (1-dot(L, H)) ,5.0)
}

 float termino_geometrico (const vec3 L,
						 const vec3 V,
						 const vec3 H,
						 const vec3 N){
	float masking = (2*dot(N,H)*dot(N,V))/dot(V,H);
	float shadowing = (2*dot(N,H)*dot(N,L))/dot(V,H);
	float aux = min(masking, shadowing);
	return min(1.0, aux);
}

float beckman (const vec3 H,
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
		float NoL = lambert(normal, lightDirection);
		if (NoL>0.0){
			vec3 aux_dif = NoL * theMaterial.diffuse * theLights[i].diffuse;		//nuestro albedo
			diffuse = diffuse + aux_dif/pi;
			//specular = F(L, H)*G(L,V,H)*D(H) / 4*(N*L)*(N*V)
			vec3 H = normalize(lightDirection+viewDirection);
			float F = fresnel(lightDirection, H);
			float G = termino_geometrico(lightDirection, viewDirection, H, normal);
			float D = beckman(H, normal); 
			float NoV = lambert(normal, viewDirection);

		}
		
}

void point_light(const in int i,
				 const in vec3 position,
				 const in vec3 viewDirection,
				 const in vec3 normal,
				 inout vec3 diffuse, inout vec3 specular) {	

				 float d = distance(theLights[i].position.xyz, position);
				 float atenuacion = 1 / (theLights[i].attenuation[0] + theLights[i].attenuation[1] * d +  theLights[i].attenuation[2] * pow(d, 2.0));

				 if (atenuacion > 0.0){
					 //el vector de dirección de la luz desde su posición hasta la superficie que ilumina
					 vec3 L = vec3(0.0);
					 L = normalize(theLights[i].position.xyz - position);
					 float NoL = lambert(normal, l);
					 if (NoL > 0.0){
					 	vec3 aux_dif = NoL * theLights[i].diffuse * theMaterial.diffuse * atenuacion;
						diffuse = diffuse + aux_dif/pi;

						vec3 H = normalize(L+viewDirection);
						float F = fresnel(L, H);
						float G = termino_geometrico(L, viewDirection, H, normal);
						float D = beckman(H, normal); 
						float NoV = lambert(normal, viewDirection);
					 }
				 }
}


void spot_light(const in int i,
				const in vec3 position,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 diffuse, inout vec3 specular) {

				 float d = distance(theLights[i].position.xyz, position);
				 float atenuacion = 1 / (theLights[i].attenuation[0] + theLights[i].attenuation[1] * d +  theLights[i].attenuation[2] * pow(d, 2.0));

				 if (atenuacion > 0.0){
					 //el vector de dirección de la luz desde su posición hasta la superficie que ilumina
					 vec3 l = vec3(0.0);
					 l = normalize(theLights[i].position.xyz - position);
					 float NoL = lambert(normal, l);
					 if (NoL > 0.0){
					 	vec3 aux_dif = NoL * theLights[i].diffuse * theMaterial.diffuse * atenuacion;
						diffuse = diffuse + aux_dif/pi;

						vec3 H = normalize(L+viewDirection);
						float F = fresnel(L, H);
						float G = termino_geometrico(L, viewDirection, H, normal);
						float D = beckman(H, normal); 
						float NoV = lambert(normal, viewDirection);
					 }
				 }
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
	diffuse = kd * diffuse;
	specular = (1 - kd) * specular;
	vec4 f_color = vec4(diffuse+specular, 1.0);
	//gl_FragColor = f_color * texture2D(texture0, f_texCoord);	//vec4
	gl_FragColor = 
}