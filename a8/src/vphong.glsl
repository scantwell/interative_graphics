attribute vec3 vPosition;
attribute vec3 vNormal;

varying vec3 fN;
varying vec3 fE;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
	fN = vNormal;
	fE = (model * vec4(vPosition, 1.0f)).xyz;
	gl_Position = projection * view * model * vec4(vPosition, 1.0);
}