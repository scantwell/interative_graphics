attribute vec3 vColor;
attribute vec4 vPosition;
varying vec3 frag_color;

void
main()
{
    frag_color = vColor;
    gl_Position = vPosition;
}
