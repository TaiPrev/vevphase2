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
	return max(0.0, dot(n,l));
}

float specular_channel(const vec3 n,
					   const vec3 l,
					   const vec3 v,
					   float m) {
	return 1.0;
}

//luz direccional
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

//luz posicional
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
				//float d = distance(theLights[i].position.xyz, position);

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
	//V = direccción de la vista, N = normal
	//normal del vértice en el espacio de la cámara
	vec4 N4 = modelToCameraMatrix * vec4(v_normal, 0.0);

	vec4 positionEye = modelToCameraMatrix * vec4(v_position, 1.0);

	//hacer lo mismo para la dirección de la vista
	vec4 V4 = (0.0, 0.0, 0.0, 1.0) -  positionEye;

	 for(int i=0; i < active_lights_n; ++i) {
		if(theLights[i].position.w == 0.0) {
		  // direction light
		  vec3 L = normalize(-theLights[i].position.xyz);
		  direction_light(i, L, normalize(V4.xyz), normalize(N4.xyz), diffuse, specular);
		} 
		  else {
		  if (theLights[i].cosCutOff == 0.0) {
			// point light
			point_light(i, positionEye.xyz, normalize(V4.xyz), normalize(N4.xyz), diffuse, specular);
		  } 
		  else {
			// spot light
			spot_light(i, positionEye.xyz, normalize(V4.xyz), normalize(N4.xyz), diffuse, specular);
		  }
		}
	 }

	f_color = vec4(diffuse+specular, 1.0);
	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
	f_texCoord = v_texCoord;
}