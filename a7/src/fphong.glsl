varying vec3 fN;
varying vec3 fE;
varying vec3 fL[3];

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

const int NUM_LIGHTS = 3;
uniform Light lights[3];
uniform Material material;

void
main()
{
	vec3 diffuse = vec3(0, 0, 0);
	vec3 specular = vec3(0, 0, 0);
	vec3 ambient = vec3(0, 0, 0);
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		vec3 L, H, diffuseProduct, specularProduct;
		float Kd, Ks;

		diffuseProduct = lights[i].diffuse * material.diffuse;
		specularProduct = lights[i].specular * material.specular;

		L = normalize(fL[i]);
		H = normalize(L + E);
		Kd = max(dot(L, N), 0.0);
		Ks = pow(max(dot(N, H), 0.0), material.shininess); 

		diffuse += Kd * diffuseProduct;
		specular += Ks * specularProduct;
		ambient += lights[i].ambient * material.ambient;
	}

	gl_FragColor = vec4(min(ambient, 1) + min(diffuse, 1) + min(specular, 1), 1.0);


}
