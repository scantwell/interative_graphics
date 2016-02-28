attribute vec3 vPosition;
attribute vec3 vColor;
varying vec3 vFragColors;

uniform mat4 modelView;
uniform mat4 projection;

void
main()
{
	vFragColors = vColor;
	gl_Position = projection * modelView * vec4(vPosition, 1.0);
}