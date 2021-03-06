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

float lambert(const vec3 n, const vec3 l) {
	return max(0.0, dot(n,l));
}

float specular_channel(const vec3 n,
					   const vec3 l,
					   const vec3 v,
					   float m) {
	return 1.0;
}

//calcular la aportación en el PIXEL

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

						diffuse = diffuse + NoL * theLights[i].diffuse * theMaterial.diffuse * atenuacion;
						vec3 r = 2 * NoL * normal - l;
					 	float aux = pow(dot(r, viewDirection), theMaterial.shininess);
					 	specular = specular +  NoL * max(0.0, aux) * theMaterial.specular * theLights[i].specular * atenuacion;
					 }
				 }
}

// Note: no attenuation in spotlights

void spot_light(const in int i,
				const in vec3 position,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 diffuse, inout vec3 specular) {

				vec3 l = vec3(0.0);
				 l = normalize(theLights[i].position.xyz - position);

				float cSpot = dot(-l, theLights[i].spotDir);
				if(cSpot > theLights[i].cosCutOff){
					cSpot = pow(cSpot, theLights[i].exponent);
					float NoL = lambert(normal, l);
					if(NoL>0.0){
						diffuse = diffuse + NoL * theLights[i].diffuse * theMaterial.diffuse * cSpot;
						vec3 r = 2 * NoL * normal - l;
						float aux = pow(dot(r, viewDirection), theMaterial.shininess);
						specular = specular + NoL * max(0.0, aux) * theMaterial.specular * theLights[i].specular * cSpot;
					}
				}
}

void main() {
	//acumuladores
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	//NORMALIZACIONES DE LOS VARYING
	vec3 P = f_position;
	vec3 V = normalize(f_viewDirection);
	vec3 N = normalize(f_normal);

	for(int i=0; i < active_lights_n; ++i) {
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

	//from pervertex vert & frag:
	//f_color = vec4(diffuse+specular, 1.0);
	//gl_FragColor = f_color * texture2D(texture0, f_texCoord);

	//original: 
	//gl_FragColor = vec4(1.0);
	//new:
	gl_FragColor = f_color * texture2D(texture0, f_texCoord);
}
