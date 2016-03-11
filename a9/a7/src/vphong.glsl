attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;

varying vec3 fN;
varying vec3 fE;
varying vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
	fN = vNormal;
	fE = (model * vec4(vPosition, 1.0f)).xyz;
	gl_Position = projection * view * model * vec4(vPosition, 1.0);
	TexCoord = vTexCoord;
}