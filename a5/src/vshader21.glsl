attribute vec3 vPosition;
attribute vec3 vNormal;
varying vec3 vFragColors;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light1;
uniform Light light2;
//uniform mat4 transform;
uniform mat4 modelView;
uniform mat4 projection;

void
main()
{
	vec3 diffuseLight1 = light1.diffuse * material.diffuse;
	vec3 diffuseLight2 = light2.diffuse * material.diffuse;

	vec3 pos = (modelView * vec4(vPosition, 1.0)).xyz;

	vec3 L1 = normalize((modelView * vec4(light1.position, 1.0)).xyz - pos);
	vec3 E1 = normalize(-pos);
	vec3 H1 = normalize(L1 + E1);

	vec3 L2 = normalize((modelView * vec4(light2.position, 1.0)).xyz - pos);
	vec3 E2 = normalize(-pos);
	vec3 H2 = normalize(L2 + E2);

	vec3 N = normalize(modelView * vec4(vNormal, 0.0)).xyz;

	float Kd = max(dot(L1,N), 0.0);
	vec3 diffuse = Kd * diffuseLight1;
	
	float Kd2 = max(dot(L2,N), 0.0);
	diffuse += Kd2 * diffuseLight2;

	vFragColors = diffuse; //diffuseLight1;
	gl_Position = projection * modelView * vec4(vPosition, 1.0);
	//vFragColors = vec4(material.diffuse, 1.0);
    //gl_Position = transform * vec4(vPosition, 1.0);
}
