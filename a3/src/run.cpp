// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <vector>
#include <ctime>
#include "multi_square.h"
#include "ellipse.h"
#include "triangle.h"
#include "circle.h"

GLuint PROGRAM, SUBPROGRAM, PROGRAM2;
MultiSquare * SQUARE;
Triangle * TRIANGLE;
Circle * CIRCLE;


std::vector<Circle*> mainCircleModels;
Ellipse* subModel;
int mainWindow, subWindow, mainWindow2;
bool isAnimated = true;

mat4 TRIANGLE_TRANS;
int theta_triangle = 0;
clock_t TRIANGLE_CLOCK = clock();

mat4 CIRCLE_TRANS;
double CIRCLE_SCALE = 1;
bool CIRCLE_SCALE_UP = false;
clock_t CIRCLE_CLOCK = clock();

mat4 SQUARE_TRANS;
int theta_square = 0;
clock_t SQUARE_CLOCK = clock();

//--------------------------------------------------------------------------

void
init(void)
{
	glutSetWindow(mainWindow);
	// Load shaders and use the resulting shader program
	PROGRAM = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(PROGRAM);

	glClearColor(0, 0, 0, 1.0);

	SQUARE = new MultiSquare(vec3(1,1,1));

	glutSetWindow(subWindow);
	
	SUBPROGRAM = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(SUBPROGRAM);

	glClearColor(0, 1, 0, 1.0);

	subModel = new Ellipse(vec3(0, 0, 0), 0.5, 0.3, vec3(1, 0, 0));

	glutSetWindow(mainWindow2);

	PROGRAM2 = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(PROGRAM2);

	glClearColor(0, 0, 0, 1.0);

	TRIANGLE = new Triangle(vec3(-0.4, -0.4, 0), vec3(0.4, -0.4, 0), vec3(0, 0.8, 0), vec3(1, 0, 0));
	CIRCLE = new Circle(vec3(0.8, 0.5, 0), .2, vec3(1, 0, 0));

}

//----------------------------------------------------------------------------

void
displayMainWindow(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	GLuint transformLoc = glGetUniformLocation(PROGRAM, "transform");

	for (auto it = mainCircleModels.begin(); it != mainCircleModels.end(); ++it)
	{
		mat4 matrix = Translate((*it)->center) * Scale(.5, .5, 0) * Translate(-1 * (*it)->center);
		int size = (*it)->meshes[0].vertices.size() - 1;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mat4());
		(*it)->Draw(PROGRAM);
	}
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, SQUARE_TRANS);
	SQUARE->Draw(PROGRAM);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mat4());
	glutSwapBuffers();
}

void
displaySubWindow(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	GLuint transformLoc = glGetUniformLocation(PROGRAM, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mat4());
	subModel->Draw(SUBPROGRAM);
	glutSwapBuffers();
}

void
displayMainWindow2(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	GLuint transformLoc = glGetUniformLocation(PROGRAM, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, TRIANGLE_TRANS);

	TRIANGLE->Draw(PROGRAM2);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mat4());
	CIRCLE->Draw(PROGRAM2);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
changeWindow2Colors(vec3 color)
{
	delete TRIANGLE;
	delete CIRCLE;
	TRIANGLE = new Triangle(vec3(-0.4, -0.4, 0), vec3(0.4, -0.4, 0), vec3(0, 0.8, 0), color);
	CIRCLE = new Circle(vec3(0.8, 0.5, 0), .2, color);
	glutSetWindow(mainWindow2);
	glutPostRedisplay();
}

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
		changeWindow2Colors(vec3(1, 0, 0));
		break;
	case 'g':
		changeWindow2Colors(vec3(0, 1, 0));
		break;
	case 'b':
		changeWindow2Colors(vec3(0, 0, 1));
		break;
	case 'y':
		changeWindow2Colors(vec3(1, 1, 0));
		break;
	case 'o':
		changeWindow2Colors(vec3(1, 0.5, 0));
		break;
	case 'p':
		changeWindow2Colors(vec3(1, 0, 1));
		break;
	case 'w':
		changeWindow2Colors(vec3(1, 1, 1));
		break;
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
}

void
mouse(int button, int state, int x, int y)
{
	if (state == 1)
	{
		int sign_x = 1;
		int sign_y = 1;

		if (x < 256)
			sign_x = -1;
		if (y > 256)
			sign_y = -1;
		double view_x = (abs(256.0 - x) / 256.0) * sign_x;
		double view_y = (abs(256.0 - y) / 256.0) * sign_y;

		glutSetWindow(mainWindow);
		mainCircleModels.push_back(new Circle(vec3(view_x, view_y, 0), .2, vec3((rand() % 10)/10.0, (rand() % 10) / 10.0, (rand() % 10) / 10.0)));
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------


void
idle(void)
{
	clock_t now = clock();
	if (isAnimated)
	{
		if ((double(now - SQUARE_CLOCK) / CLOCKS_PER_SEC) > 0.004)
		{
			theta_square -= 1;
			if (theta_square < -360)
				theta_square = 0;
			SQUARE_TRANS = RotateZ(theta_square);
			SQUARE_CLOCK = now;
			glutSetWindow(mainWindow);
			glutPostRedisplay();
		}
		if ((double(now - TRIANGLE_CLOCK) / CLOCKS_PER_SEC) > 0.004)
		{
			theta_triangle += 1;
			if (theta_triangle > 360)
				theta_triangle = 0;
			TRIANGLE_TRANS = Translate(TRIANGLE->center) * RotateZ(theta_triangle) * Translate(-1 * TRIANGLE->center);
			TRIANGLE_CLOCK = now;
			glutSetWindow(mainWindow2);
			glutPostRedisplay();
		}
		if ((double(now - CIRCLE_CLOCK) / CLOCKS_PER_SEC) > 0.1)
		{
			CIRCLE_SCALE_UP = false;
			if (CIRCLE_SCALE < 0.3)
			{
				CIRCLE_SCALE_UP = true;
			}

			if (CIRCLE_SCALE_UP)
			{
				CIRCLE_SCALE += 0.1;
			}
			else
			{
				CIRCLE_SCALE -= 0.1;
			}
			CIRCLE_TRANS = Scale(CIRCLE_SCALE, CIRCLE_SCALE, 0);
			CIRCLE_CLOCK = now;
			glutPostRedisplay();
		}
	}
}

void
processMenuEvents(int id)
{
	switch (id)
	{
	case 1:
		isAnimated = false;
		break;
	case 2:
		isAnimated = true;
		break;
	case 31:
		delete SQUARE;
		SQUARE = new MultiSquare(vec3(1,1,1));
		break;
	case 32:
		delete SQUARE;
		SQUARE = new MultiSquare(vec3(1, 0, 0));
		break;
	case 33:
		delete SQUARE;
		SQUARE = new MultiSquare(vec3(0, 1, 0));
		break;
	case 4:
		glutSetWindow(subWindow);
		glClearColor(1, 1, 1, 1.0);
		break;
	case 5:
		glutSetWindow(subWindow);
		glClearColor(0, 0, 0, 1.0);
		break;
	case 6:
		glutSetWindow(subWindow);
		glClearColor(1, 0, 0, 1.0);
		break;
	case 7:
		glutSetWindow(subWindow);
		glClearColor(0, 1, 0, 1.0);
		break;
	case 8:
		glutSetWindow(subWindow);
		glClearColor(0, 0, 1, 1.0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void createMainWindow()
{
	mainWindow = glutCreateWindow("Assignment 3");
	glutPositionWindow(0, 0);
	glutDisplayFunc(displayMainWindow);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	int submenu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("White", 31);
	glutAddMenuEntry("Red", 32);
	glutAddMenuEntry("Green", 33);
	glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Stop Animation", 1);
	glutAddMenuEntry("Start Animation", 2);
	glutAddSubMenu("Square Color", submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void createMainWindow2()
{
	mainWindow2 = glutCreateWindow("window 2");
	glutPositionWindow(512, 0);
	glutReshapeWindow(512, 256);
	glutDisplayFunc(displayMainWindow2);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
}

void createMainSubWindow()
{
	subWindow = glutCreateSubWindow(mainWindow, 0, 0, 200, 150);
	glutDisplayFunc(displaySubWindow);

	glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("White", 4);
	glutAddMenuEntry("Black", 5);
	glutAddMenuEntry("Red", 6);
	glutAddMenuEntry("Green", 7);
	glutAddMenuEntry("Blue", 8);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int
main(int argc, char **argv)
{
	std::cout << "This program implements the following features: \n "
		"- Double Buffering \n - Main window squares \n "
		"- Sub Window \n "
		"- Sub Window menu (right on sub window to use.) changes the background color of the subwindow.\n "
		"- Second window which includes a triangle and a circle \n "
		"- Second window input with key strokes, supported input to change color of objects "
		"'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'o' - orange, 'p' - purple, 'w' - white \n "
		"- Animated square and triangle \n "
		"- Main menu - (right click to use) which includes functionality to change the color of the rotating squares and starting/stopping animations on both windows.\n"
		"- Adding circles to the main window is possible.\n\n"
		"Notes: I was unable to implement the 'breathing' functionality for the circles because I could not get Scale() function to work.\n Enjoy."<< std::endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	createMainWindow();
	createMainSubWindow();
	createMainWindow2();

	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutMainLoop();
	return 0;
}