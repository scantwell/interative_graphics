// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <model.h>

struct Attribute
{
	vec3 position;
	double delta;
	double factor;
	Attribute(vec3 position, double delta, double factor) : position(position),
		delta(delta),
		factor(factor) {}
};

GLuint PROGRAM;
Model * CUBE;

Attribute ROTATE(vec3(0, 0, 0), 5, 5);
Attribute TRANSLATE(vec3(0, 0, 0), 0.1, 0.1);
Attribute SCALE(vec3(1, 1, 1), 0.1, 0.1);

Attribute * CURRENT = &ROTATE;

//--------------------------------------------------------------------------

void
reset()
{
	ROTATE = Attribute(vec3(0, 0, 0), 5, 5);
	TRANSLATE = Attribute(vec3(0, 0, 0), 0.1, 0.1);
	SCALE = Attribute(vec3(1, 1, 1), 0.1, 0.1);
	Attribute * CURRENT = &ROTATE;
}

void
init(void)
{
	// Load shaders and use the resulting shader program
	PROGRAM = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(PROGRAM);
	glClearColor(0, 0, 0, 1.0);
	CUBE = new Model();
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);
	GLuint transformLoc = glGetUniformLocation(PROGRAM, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_TRUE, Translate(TRANSLATE.position) * RotateZ(ROTATE.position.z) * RotateY(ROTATE.position.y) * RotateX(ROTATE.position.x) * Scale(SCALE.position));
	CUBE->Draw(PROGRAM);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mat4());
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	std::cout << key << std::endl;
	switch (key) {
	case 'q': // x up
		CURRENT->position.x += CURRENT->delta;
		break;
	case 'a': // x down
		CURRENT->position.x -= CURRENT->delta;
		break;
	case 'w': // y up
		CURRENT->position.y += CURRENT->delta;
		break;
	case 's': // y down
		CURRENT->position.y -= CURRENT->delta;
		break;
	case 'e': // z up
		CURRENT->position.z += CURRENT->delta;
		break;
	case 'd': // z down
		CURRENT->position.z -= CURRENT->delta;
		break;
	case ' ':
		reset();
		break;
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
	std::cout << button << " " << state << " " << x << " " << y << std::endl;
	if (button == 3)
	{
		CURRENT->delta += CURRENT->factor;
	}
	else if (button == 4)
	{
		CURRENT->delta -= CURRENT->factor;
	}
}

void
processMenuEvents(int id)
{
	switch (id)
	{
	case 1:
		CURRENT = &ROTATE;
		break;
	case 2:
		CURRENT = &SCALE;
		break;
	case 3:
		CURRENT = &TRANSLATE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
//----------------------------------------------------------------------------


int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Assignment 4");
	glutPositionWindow(0, 0);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Rotate", 1);
	glutAddMenuEntry("Scale", 2);
	glutAddMenuEntry("Translate", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutMainLoop();
	return 0;
}