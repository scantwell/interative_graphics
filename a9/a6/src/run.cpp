// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <model.h>
#include <sstream>
#include <string>


struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	Light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular) :
		position(position),
		ambient(ambient),
		diffuse(diffuse),
		specular(specular) {}
};

struct Camera
{
	vec4 at = vec4(0, 0, 0, 1.0);
	vec4 up = vec4(0, 1.0, 0, 1.0);
	double speed = 0.1;
	double radius = 1.8;
	double height = 0;
};

std::vector<Light> LIGHTS;

Material EMERALD = Material(vec3(0.0215, 0.1745, 0.0215), vec3(0.07568, 0.61424, 0.07568), vec3(0.633, 0.727811, 0.633), 5);
Material GREEN_PLASTIC = Material(vec3(0.0, 0.0, 0.0), vec3(0.1, 0.35, 0.1), vec3(0.45, 0.55, 0.45), 0.25);
Material GREEN_RUBBER = Material(vec3(0.0, 0.05, 0.0), vec3(0.4, 0.5, 0.4), vec3(0.04, 0.7, 0.04), 0.078125);

GLuint PROGRAM, GOURAUD, PHONG;
Model * MODEL;
Camera * CAMERA;
bool IS_ANIMATED = false;
bool PARALLEL = true;
bool PERSPECTIVE = false;

float STEPS = 100.;
//
clock_t CLOCK = clock();
double THETA = 0;

double LIGHT_RADIUS = 3;
double LIGHT_THETA = 0;

//bunny-59k
vec3 LOWER_BOUND = vec3(-0.95, 0, -0.62);
vec3 UPPER_BOUND = vec3(0.61, 1.87, 0.59);
vec3 CENTER_POINT = (LOWER_BOUND + UPPER_BOUND) / 2.0;


//--------------------------------------------------------------------------

void
load_scene_lighting(GLuint program)
{
	GLint vpositionLoc, positionLoc, ambientLoc, diffuseLoc, specularLoc;
	int count = 0;
	for (auto it = LIGHTS.begin(); it != LIGHTS.end(); ++it)
	{
		std::stringstream ss;
		std::string str;

		ss << "lightPositions[" << count << "]";
		str = ss.str();
		vpositionLoc = glGetUniformLocation(program, str.c_str());
		ss.str("");

		ss << "lights[" << count << "].position";
		str = ss.str();
		positionLoc = glGetUniformLocation(program, str.c_str());
		ss.str("");

		ss << "lights[" << count << "].ambient";
		str = ss.str();
		ambientLoc = glGetUniformLocation(program, str.c_str());
		ss.str("");

		ss << "lights[" << count << "].diffuse";
		str = ss.str();
		diffuseLoc = glGetUniformLocation(program, str.c_str());
		ss.str("");

		ss << "lights[" << count << "].specular";
		str = ss.str();
		specularLoc = glGetUniformLocation(program, str.c_str());
		ss.str("");

		glUniform3f(vpositionLoc, it->position.x, it->position.y, it->position.z);
		glUniform3f(positionLoc, it->position.x, it->position.y, it->position.z);
		glUniform3f(ambientLoc, it->ambient.x, it->ambient.y, it->ambient.z);
		glUniform3f(diffuseLoc, it->diffuse.x, it->diffuse.y, it->diffuse.z);
		glUniform3f(specularLoc, it->specular.x, it->specular.y, it->specular.z);
		count++;
	}
}

void
init(void)
{
	// Load shaders and use the resulting shader program
	PHONG = InitShader("vphong.glsl", "fphong.glsl");
	GOURAUD = InitShader("vgouraud.glsl", "fgouraud.glsl");
	glUseProgram(GOURAUD);
	glClearColor(0, 0, 0, 1.0);

	MODEL = new Model("../smf/bunny_69k.smf");
	MODEL->SetMaterial(GREEN_PLASTIC);

	LIGHTS.push_back(Light(vec3(LOWER_BOUND.x - 3.0, CENTER_POINT.y, LOWER_BOUND.z + 3), vec3(0.1, 0.1, 0.1), vec3(0.3, 0.3, 0.3), vec3(0.1, 0.1, 0.1)));
	LIGHTS.push_back(Light(vec3(CENTER_POINT.x, UPPER_BOUND.y + 5, CENTER_POINT.z), vec3(0.1, 0.1, 0.1), vec3(0.4, 0.4, 0.4), vec3(0.1, 0.1, 0.1)));
	LIGHTS.push_back(Light(vec3(0, 0, 0), vec3(0.1, 0.1, 0.1), vec3(0.5, 0.5, 0.51), vec3(0.05, 0.05, 0.05)));
	CAMERA = new Camera();

	CAMERA->at = CENTER_POINT;
	CAMERA->height = CENTER_POINT.y;

	load_scene_lighting(GOURAUD);
	load_scene_lighting(PHONG);

	PROGRAM = GOURAUD;
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);

	vec4 eye = vec4(sin(THETA) * CAMERA->radius, CAMERA->height, cos(THETA) * CAMERA->radius, 1);
	LIGHTS[2].position = vec3(eye.x, eye.y, eye.z);

	load_scene_lighting(PROGRAM);

	mat4 modelView = LookAt(eye, CAMERA->at, CAMERA->up);
	GLuint  modelViewLoc = glGetUniformLocation(PROGRAM, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);
	GLuint  eyeLoc = glGetUniformLocation(PROGRAM, "eyePosition");
	glUniform3f(eyeLoc, eye.x, eye.y, eye.z);

	mat4 projection;
	if (PARALLEL)
	{
		//left right bottom top zNear zFar
		projection = Ortho(-1, 1, -1, 1, -1, 5);
	}
	else
	{
		projection = Perspective(45.0f, 1, 0.1, 100);
	}
	GLuint  projectionLoc = glGetUniformLocation(PROGRAM, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);

	GLuint  steps = glGetUniformLocation(PROGRAM, "steps");
	glUniform1f(steps, STEPS);

	MODEL->Draw(PROGRAM);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
updateLight()
{
	LIGHTS[0].position = vec3(sin(LIGHT_THETA) * LIGHT_RADIUS, LIGHTS[0].position.y, cos(LIGHT_THETA) * LIGHT_RADIUS);
}

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q': // up Height
		CAMERA->height += 0.1;
		break;
	case 'a': // down Height
		CAMERA->height -= 0.1;
		break;
	case 'w': // up Radius
		CAMERA->radius += 0.1;
		break;
	case 's': // down Radius
		CAMERA->radius -= 0.1;
		break;
	case 'e': // up Speed
		CAMERA->speed += 1;
		break;
	case 'd': // down Speed
		CAMERA->speed -= 1;
		break;
	case 'u': // up height
		LIGHTS[0].position.y += 0.1;
		updateLight();
		break;
	case 'j': // down height
		LIGHTS[0].position.y -= 0.1;
		updateLight();
		break;
	case 'i': // up radius
		LIGHT_RADIUS += 0.1;
		updateLight();
		break;
	case 'k': // down radius
		LIGHT_RADIUS -= 0.1;
		updateLight();
		break;
	case 'o': // angle
		LIGHT_THETA += 0.1;
		updateLight();
		break;;
	case 'l': // angle down
		LIGHT_THETA -= 0.1;
		updateLight();
		break;
	case ' ':
		delete CAMERA;
		CAMERA = new Camera();
		break;
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

void
processMenuEvents(int id)
{
	switch (id)
	{
	case 1:
		PARALLEL = true;
		PERSPECTIVE = false;
		break;
	case 2:
		PARALLEL = false;
		PERSPECTIVE = true;
		break;
	case 3:// Start
		IS_ANIMATED = true;
		break;
	case 4: //Stop
		IS_ANIMATED = false;
		break;
	case 5: // Phong Shading
		PROGRAM = PHONG;
		glUseProgram(PROGRAM);
		break;
	case 6: // Gouraud Shading
		PROGRAM = GOURAUD;
		glUseProgram(PROGRAM);
		break;
	case 7: // EMERALD Material
		MODEL->SetMaterial(EMERALD);
		break;
	case 8: // GREEN Plastic Material
		MODEL->SetMaterial(GREEN_PLASTIC);
		break;
	case 9: // GREEN RUBBER Material
		MODEL->SetMaterial(GREEN_RUBBER);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void
idle()
{
	clock_t now = clock();
	if (IS_ANIMATED && (double(now - CLOCK) / CLOCKS_PER_SEC) > 0.004)
	{
		THETA += CAMERA->speed * DegreesToRadians;
		glutPostRedisplay();
		CLOCK = now;
	}
}
//----------------------------------------------------------------------------


void
createMenu() 
{
	int projection = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Parallel Projection", 1);
	glutAddMenuEntry("Perspective Projection", 2);

	int camera = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Start Rotation", 3);
	glutAddMenuEntry("Stop Rotation", 4);

	int shading = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Phong", 5);
	glutAddMenuEntry("Gouraud", 6);

	int material = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Emerald", 7);
	glutAddMenuEntry("Green Plastic", 8);
	glutAddMenuEntry("Green Rubber", 9);


	glutCreateMenu(processMenuEvents);
	glutAddSubMenu("Projection Type", projection);
	glutAddSubMenu("Camera", camera);
	glutAddSubMenu("Shading", shading);
	glutAddSubMenu("Material", material);
}

int
main(int argc, char **argv)
{
	std::cout << "\n\nAssignment 6 \n\nFeatures:\n\n"
		" - Right Click Menu\n"
		"	- Projection\n"
		"		- Parallel Projection\n"
		"		- Perspective Projection\n"
		"	- Camera\n"
		"		- Start Rotation\n"
		"		- Stop Rotation\n\n"
		"	- Shading \n"
		"		- Phong \n"
		"		- Gouraud \n\n"
		"	- Material\n"
		"		- Emerald\n"
		"		- Green Plastic\n"
		"		- Green Rubber\n\n"
		" - Keys\n"
		"	'q' - increases height of camera\n"
		"	'a' - decreases height of camera\n"
		"	'w' - increases radius of camera\n"
		"	's' - decreases radius of camera\n"
		"	'e' - increases speed of camera rotation\n"
		"	'd' - decreases speed of camera rotation\n"
		"	'u' - increases height of light\n"
		"	'j' - decreases height of light\n"
		"	'i' - increases radius of light\n"
		"	'k' - decreases radius of light\n"
		"	'o' - increases angle of light\n"
		"	'l' - decreases angle of light\n"
		"	SPACE - Resets configuration\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Assignment 6");
	glutPositionWindow(0, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	createMenu();

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);

	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutMainLoop();
	return 0;
}