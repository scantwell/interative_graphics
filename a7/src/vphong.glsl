attribute vec3 vPosition;
attribute vec3 vNormal;

varying vec3 fN;
varying vec3 fE;
varying vec3 fL[3];

const int NUM_LIGHTS = 3;
uniform vec3 lightPositions[3];
uniform mat4 modelView;
uniform mat4 projection;
uniform vec3 eyePosition;

void
main()
{
	fN = vNormal;
	fE = eyePosition - vPosition;
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
		fL[i] = lightPositions[i] - vPosition.xyz;
	}
	gl_Position = projection * modelView * vec4(vPosition, 1.0);
}