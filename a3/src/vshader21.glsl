attribute vec3 vPosition;
attribute vec3 vColors;
varying vec3 vFragColors;

uniform mat4 transform;

void
main()
{
	vFragColors = vColors;
    gl_Position = transform * vec4(vPosition, 1.0f);
}
