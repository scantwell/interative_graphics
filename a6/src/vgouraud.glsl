attribute vec3 vPosition;
attribute vec3 vNormal;
varying vec3 vFragColors;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int camera_light;
};

uniform Material material;
const int NUM_LIGHTS = 3;
uniform Light lights[NUM_LIGHTS];
uniform mat4 modelView;
uniform mat4 projection;
uniform vec3 eyePosition;

void
main()
{
	vec3 diffuse = vec3(0, 0, 0);
	vec3 specular = vec3(0, 0, 0);
	vec3 ambient = vec3(0, 0, 0);
	vec3 N = normalize(modelView * vec4(vNormal, 0.0)).xyz;

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		vec3 L, E, H, diffuseProduct, specularProduct, pos;
		float Kd, Ks;

		diffuseProduct = lights[i].diffuse * material.diffuse;
		specularProduct = lights[i].specular * material.specular;

		pos = (modelView * vec4(vPosition, 1.0)).xyz;

		L = normalize((modelView * vec4(lights[i].position, 1.0)).xyz - pos);
		E = normalize(-pos);
		H = normalize(L + E);
		Kd = max(dot(L, N), 0.0);
		Ks = pow(max(dot(N, H), 0.0), material.shininess); 

		diffuse += Kd * diffuseProduct;
		specular += Ks * specularProduct;
		ambient += lights[i].ambient * material.ambient;
	}

	vFragColors = min(ambient, 1) + min(diffuse, 1) + min(specular, 1);
	gl_Position = projection * modelView * vec4(vPosition, 1.0);
}
