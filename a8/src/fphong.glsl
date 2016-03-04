varying vec3 fN;
varying vec3 fE;


struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

struct Light
{
	vec3 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int camera_light;
};

const int NUM_LIGHTS = 1;
uniform Light lights[NUM_LIGHTS];
uniform Material material;
uniform float const_color;
uniform vec3 vColor;
uniform vec3 eyePosition;

void
main()
{
	if (const_color == 1.0f)
	{
		gl_FragColor = vec4(vColor, 1);
	}
	else
	{
		vec4 diffuse = vec4(0, 0, 0, 1);
		vec4 specular = vec4(0, 0, 0, 1);
		vec4 ambient = vec4(0, 0, 0, 1);
		vec3 N = normalize(fN);
		vec3 E = normalize(fE);

		for (int i = 0; i < NUM_LIGHTS; i++)
		{
			vec4 L, H, diffuseProduct, specularProduct;
			float Kd, Ks;

			//ambient
			ambient += lights[i].ambient * material.ambient;

			// diffuse
			vec3 lightDir = normalize(lights[i].position - E);
			float diff = max(dot(N, lightDir), 0.0);
			diffuse += diff * lights[i].diffuse * material.diffuse;
		
			// specular
			vec3 viewDir = normalize(eyePosition - E);
			vec3 reflectDir = reflect(-lightDir, N);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			if (dot(lightDir, N) > 0.0)
			{
				specular += spec * lights[i].specular * material.specular;  
			}
		}
		gl_FragColor = min(ambient, 1) + min(diffuse, 1) + min(specular, 1);
	}
}
