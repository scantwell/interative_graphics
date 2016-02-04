attribute vec4 vPosition;
attribute vec4 vColors;
varying vec4 vFragColors;

uniform mat4 transform;

void
main()
{
	vFragColors = vColors;
    gl_Position = transform * vPosition;
}
