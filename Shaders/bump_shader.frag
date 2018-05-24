#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

struct material_t {
	vec3  diffuse;
	vec3  specular;
	float alpha;
	float shininess;
};

struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
};

uniform light_t theLights[4];
uniform material_t theMaterial;

uniform sampler2D texture0;
uniform sampler2D bumpmap;

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space
varying vec3 f_normal;	//normal pasada al espacio de la tangente, añadido por mi


//NOTES: DON'T USE POSITION
//		 LIGHT DIRECTIONS ARE CALCULATED AT THE VERTEX SHADER
//		 AND PASSED THROUGH A VARYING
float lambert(const vec3 n, const vec3 l) {
	return max(0.0, dot(n,l));
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

	//para la luz i-ésima se acumula el coseno que se forma en lamber por el color de la luz por el color del material
		float NoL = lambert(normal, lightDirection);	// = dot(normal, lightDirection) 
		if (NoL>0.0){
			diffuse = diffuse + NoL * theMaterial.diffuse * theLights[i].diffuse;
			vec3 r = 2 * NoL * normal - lightDirection;
			float aux = pow(dot(r, viewDirection), theMaterial.shininess);
			//CALCULO DE REFLEXIÓN ESPECULAR
			specular = specular + NoL * max(0.0, aux) * theMaterial.specular * theLights[i].specular;
		}
}

void point_light(const in int i,
				 const in vec3 lightDirection,
				 const in vec3 viewDirection,
				 const in vec3 normal,
				 inout vec3 diffuse, inout vec3 specular) {

					 float NoL = lambert(normal, lightDirection);
					 if (NoL > 0.0){
						diffuse = diffuse + NoL * theLights[i].diffuse * theMaterial.diffuse;
						vec3 r = 2 * NoL * normal - lightDirection;
					 	float aux = pow(dot(r, viewDirection), theMaterial.shininess);
					 	specular = specular +  NoL * max(0.0, aux) * theMaterial.specular * theLights[i].specular;
					 }
}

// Note: no attenuation in spotlights

void spot_light(const in int i,
				//const in vec3 lightDirection,
				const in vec3 spotDir,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 diffuse, inout vec3 specular) {

				//vec3 l = vec3(0.0);
				//l = normalize(theLights[i].position.xyz - position);

				float cSpot = dot(-spotDir, theLights[i].spotDir);
				if(cSpot > theLights[i].cosCutOff){
					cSpot = pow(cSpot, theLights[i].exponent);
					float NoL = lambert(normal, spotDir);
					if(NoL>0.0){
						diffuse = diffuse + NoL * theLights[i].diffuse * theMaterial.diffuse * cSpot;
						vec3 r = 2 * NoL * normal - spotDir;
						float aux = pow(dot(r, viewDirection), theMaterial.shininess);
						specular = specular + NoL * max(0.0, aux) * theMaterial.specular * theLights[i].specular * cSpot;
					}
				}
}


void main() {
	
	//acumuladores
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	/*
varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space
*/

	//NORMALIZACIONES DE LOS VARYING
	vec3 V = normalize(f_viewDirection);
	vec3 N = normalize(f_normal);

	//QUITAR EL FOR Y PASAR POR LAS 4 LUCES A MANO

	for(int i=0; i < active_lights_n; ++i) {
		if(theLights[i].position.w == 0.0) {
		  // direction light
		  vec3 L = normalize(f_lightDirection[i]);
		  direction_light(i, L, V, N, diffuse, specular);
		} 
		  else {
		  if (theLights[i].cosCutOff == 0.0) {
			// point light
			vec3 L = normalize(f_lightDirection[i]);
			point_light(i, L, V, N, diffuse, specular);
		  } 
		  else {
			// spot light
			vec3 spotDir = normalize(f_spotDirection[i]);
			spot_light(i, spotDir, V, N, diffuse, specular);
		  }
		}
	 }

	vec4 f_color = vec4(diffuse+specular, 1.0);
	gl_FragColor = f_color * texture2D(texture0, f_texCoord);
	

	//gl_FragColor = vec4(1.0);
}
