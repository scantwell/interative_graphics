// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <model.h>

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
	double speed = 0.5;
	double radius = 1;
	double height = 0.5;
};

Light * LIGHT1;
Light * LIGHT2;

GLuint PROGRAM;
Model * CUBE;
Camera * CAMERA;
bool IS_ANIMATED = false;
bool PARALLEL = true;
bool PERSPECTIVE = false;
clock_t CLOCK = clock();
double THETA = 0;
//--------------------------------------------------------------------------

void
init(void)
{
	// Load shaders and use the resulting shader program
	PROGRAM = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(PROGRAM);
	glClearColor(0, 0, 0, 1.0);

	CUBE = new Model("../smf/dragon-50000.smf");

	LIGHT1 = new Light(vec3(-1.0, 0.5, 1), vec3(0, 0, 0), vec3(1.0, 1.0, 1.0), vec3(0, 0, 0));
	LIGHT2 = new Light(vec3(1.0, 0.5, -1), vec3(0, 0, 0), vec3(1.0, 1.0, 1.0), vec3(0, 0, 0));
	CAMERA = new Camera();

	GLint positionLoc = glGetUniformLocation(PROGRAM, "light1.position");
	GLint ambientLoc = glGetUniformLocation(PROGRAM, "light1.ambient");
	GLint diffuseLoc = glGetUniformLocation(PROGRAM, "light1.diffuse");
	GLint specularLoc = glGetUniformLocation(PROGRAM, "light1.specular");
	glUniform3f(positionLoc, LIGHT1->position.x, LIGHT1->position.y, LIGHT1->position.z);
	glUniform3f(ambientLoc, LIGHT1->ambient.x, LIGHT1->ambient.y, LIGHT1->ambient.z);
	glUniform3f(diffuseLoc, LIGHT1->diffuse.x, LIGHT1->diffuse.y, LIGHT1->diffuse.z);
	glUniform3f(specularLoc, LIGHT1->specular.x, LIGHT1->specular.y, LIGHT1->specular.z);

	positionLoc = glGetUniformLocation(PROGRAM, "light2.position");
	ambientLoc = glGetUniformLocation(PROGRAM, "light2.ambient");
	diffuseLoc = glGetUniformLocation(PROGRAM, "light2.diffuse");
	specularLoc = glGetUniformLocation(PROGRAM, "light2.specular");
	glUniform3f(positionLoc, LIGHT2->position.x, LIGHT2->position.y, LIGHT2->position.z);
	glUniform3f(ambientLoc, LIGHT2->ambient.x, LIGHT2->ambient.y, LIGHT2->ambient.z);
	glUniform3f(diffuseLoc, LIGHT2->diffuse.x, LIGHT2->diffuse.y, LIGHT2->diffuse.z);
	glUniform3f(specularLoc, LIGHT2->specular.x, LIGHT2->specular.y, LIGHT2->specular.z);
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);
	
	vec4 eye = vec4(sin(THETA) * CAMERA->radius, CAMERA->height, cos(THETA) * CAMERA->radius, 1);

	mat4 modelView = LookAt(eye, CAMERA->at, CAMERA->up);
	GLuint  modelViewLoc = glGetUniformLocation(PROGRAM, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);

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

	CUBE->Draw(PROGRAM);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

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


int
main(int argc, char **argv)
{
	std::cout << "\n\nAssignment 5 \n\nFeatures:\n\n"
		" - Right Click Menu\n"
		"	- Parallel Projection\n"
		"	- Perspective Projection\n"
		"	- Start Rotation\n"
		"	- Stop Rotation\n\n"
		" - Keys\n"
		"	'q' - increases height of camera\n"
		"	'a' - decreases height of camera\n"
		"	'w' - increases radius of camera\n"
		"	's' - decreases radius of camera\n"
		"	'e' - increases speed of camera rotation\n"
		"	'd' - decreases speed of camera rotation\n"
		"	SPACE - Resets configuration\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Assignment 5");
	glutPositionWindow(0, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Parallel Projection", 1);
	glutAddMenuEntry("Perspective Projection", 2);
	glutAddMenuEntry("Start Rotation", 3);
	glutAddMenuEntry("Stop Rotation", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);

	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutMainLoop();
	return 0;
}