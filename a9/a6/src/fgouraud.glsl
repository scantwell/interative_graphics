varying vec3 vFragColors;
varying vec3 vPos;
varying vec3 Eye;
varying vec3 Norm;

uniform float steps;

float stripes_movement(vec2 p, float steps) {
  return fract(p.x * steps * Norm.y * Eye.y);
}

void
main()
{
    gl_FragColor = vec4(vec3(stripes_movement(vPos.xy, steps)), 1.0);
}
