// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <ctime>
#include <string>
#include <cstdlib>
#include "model.h"

struct Light
{
	vec3 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	Light(vec3 position, vec4 ambient, vec4 diffuse, vec4 specular) :
		position(position),
		ambient(ambient),
		diffuse(diffuse),
		specular(specular) {}
};

struct Camera
{
	vec4 at = vec4(0, 0, 0, 1.0);
	vec4 up = vec4(0, 1, 0, 1.0);
	double speed = 0.1;
	double radius = 2;
	double height = 0;
};

struct Triangle
{
	vec3 position;
	vec3 color;
	Material material;
	Triangle(vec3 position, vec3 const_color, Material material) :
		position(position),
		color(const_color),
		material(material) {}
};

Camera * CAMERA;
Model * MODEL;
const int NUM_TRIANGLES = 3;
Triangle * TRIANGLES[NUM_TRIANGLES];
float FOV = 45.0f;


GLuint PROGRAM, FRAMEBUFFER, COLOR_RB, DEPTH_RB;

bool IS_ANIMATED = false;
//
clock_t CLOCK = clock();
double THETA = 0;

Material RUBY = Material(vec4(0.1745, 0.01175, 0.01175, 1), vec4(0.61424, 0.04136, 0.04136, 1), vec4(0.727811, 0.626959, 0.626959, 1), 0.6);
Material CYAN_PLASTIC = Material(vec4(0.0, 0.1, 0.06, 1), vec4(0.0, 0.50980392, 0.50980392, 1), vec4(0.50196078, 0.50196078, 0.50196078, 1), 0.25);
Material GREEN_PLASTIC = Material(vec4(0, 0, 0, 1), vec4(0.1, 0.35, 0.1, 1), vec4(0.45, 0.55, 0.45, 1), 0.25);
Material YELLOW_PLASTIC = Material(vec4(0, 0, 0, 1), vec4(0.5, 0.5, 0.0, 1), vec4(0.60, 0.60, 0.50, 1), 0.25);

Material MATERIALS[4] = { RUBY, CYAN_PLASTIC, GREEN_PLASTIC, YELLOW_PLASTIC };

std::vector<Light> LIGHTS;

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

		glUniform3f(positionLoc, it->position.x, it->position.y, it->position.z);
		glUniform4f(ambientLoc, it->ambient.x, it->ambient.y, it->ambient.z, it->ambient.w);
		glUniform4f(diffuseLoc, it->diffuse.x, it->diffuse.y, it->diffuse.z, it->diffuse.w);
		glUniform4f(specularLoc, it->specular.x, it->specular.y, it->specular.z, it->specular.w);
		count++;
	}
}

void
createFrameBuffer()
{
	GLuint color_rb, depth_rb;

	glGenFramebuffers(1, &FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, FRAMEBUFFER);
	glGenRenderbuffers(1, &COLOR_RB);
	glBindRenderbuffer(GL_RENDERBUFFER, COLOR_RB);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER, COLOR_RB);

	//-------------------------
	glGenRenderbuffers(1, &DEPTH_RB);
	glBindRenderbuffer(GL_RENDERBUFFER, DEPTH_RB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, DEPTH_RB);

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 512, 512);
}

void
init(void)
{
	// Load shaders and use the resulting shader program
	PROGRAM = InitShader("vphong.glsl", "fphong.glsl");
	glClearColor(0, 0, 0, 1.0);
	glUseProgram(PROGRAM);
	MODEL = new Model("eqlat_triangle.smf");
	TRIANGLES[0] = new Triangle(vec3(-0.5, 0.5, 0), vec3(0, 0, 1), GREEN_PLASTIC);
	TRIANGLES[1] = new Triangle(vec3(0.5, 0.5, 0), vec3(0, 1, 0), YELLOW_PLASTIC);
	TRIANGLES[2] = new Triangle(vec3(0, -0.5, 0), vec3(1, 0, 0), CYAN_PLASTIC);
	LIGHTS.push_back(Light(vec3(0, 0.5, 0), vec3(0.2, 0.2, 0.2), vec3(0.6, 0.6, 0.6), vec3(0.05, 0.05, 0.05)));

	load_scene_lighting(PROGRAM);

	CAMERA = new Camera();

	CAMERA->at = vec3(0, 0, 0);
}

//----------------------------------------------------------------------------


void
drawTriangles(bool const_color)
{
	GLuint  colorFlagLoc = glGetUniformLocation(PROGRAM, "const_color");
	GLuint  colorLoc = glGetUniformLocation(PROGRAM, "vColor");

	if (const_color)
	{
		glUniform1f(colorFlagLoc, 1.0f);
	}
	else
	{
		glUniform1f(colorFlagLoc, 0.0f);
	}

	for (int i = 0; i < NUM_TRIANGLES; ++i)
	{
		mat4 model;
		if (i == 3)
		{
			model = Translate(TRIANGLES[i]->position) * Scale(0.2, 0.2, 0.2);
			glUniform1f(colorFlagLoc, 1.0f);
		}
		else
		{
			model = Translate(TRIANGLES[i]->position) * Scale(0.5, 0.5, 0.5);
		}
		GLuint  modelLoc = glGetUniformLocation(PROGRAM, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);

		glUniform3fv(colorLoc, 1, (GLfloat*)&TRIANGLES[i]->color);
		MODEL->SetMaterial(TRIANGLES[i]->material);
		MODEL->Draw(PROGRAM);
	}
}

void
display(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FRAMEBUFFER);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FRAMEBUFFER);
	glBindRenderbuffer(GL_RENDERBUFFER, COLOR_RB);
	glBindRenderbuffer(GL_RENDERBUFFER, DEPTH_RB);

	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);

	glUseProgram(PROGRAM);
	vec4 eye = vec4(sin(THETA) * CAMERA->radius, CAMERA->height, cos(THETA) * CAMERA->radius, 1);
	mat4 modelView = LookAt(eye, CAMERA->at, CAMERA->up);
	
	GLuint  modelViewLoc = glGetUniformLocation(PROGRAM, "view");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);

	GLuint  eyeLoc = glGetUniformLocation(PROGRAM, "eyePosition");
	glUniform3f(eyeLoc, eye.x, eye.y, eye.z);

	mat4 projection = Perspective(FOV, 1, 0.1, 100);

	GLuint  projectionLoc = glGetUniformLocation(PROGRAM, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);

	LIGHTS[0].position = vec3(eye.x, eye.y, eye.z);

	load_scene_lighting(PROGRAM);

	drawTriangles(true);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FRAMEBUFFER);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawTriangles(false);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
idle()
{
	clock_t now = clock();
	if (IS_ANIMATED && (double(now - CLOCK) / CLOCKS_PER_SEC) > 0.004)
	{
		THETA += CAMERA->speed * DegreesToRadians;
		CLOCK = now;
		glutPostRedisplay();
	}
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
		FOV += 1.0;
		if (FOV > 45.0)
		{
			FOV = 45.0;
		}
		break;
	case 's': // down Radius
		FOV -= 1.0;
		if (FOV < 1.0)
		{
			FOV = 1.0;
		}
		break;
	case 'e': // up Speed
		CAMERA->speed += 1;
		break;
	case 'd': // down Speed
		CAMERA->speed -= 1;
		break;
	case ' ':
		THETA = 0;
		IS_ANIMATED = false;
		FOV = 45.0f;
		CAMERA->height = 0;
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
mouse(int button, int state, int x, int y)
{
	if (state == 1)
	{
		int view_x = x;
		int view_y = abs(512 - y);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FRAMEBUFFER);
		GLfloat myimage[512][512][4];
		glReadPixels(0, 0, 512, 512, GL_RGBA, GL_FLOAT, myimage);
		GLfloat * color = myimage[view_y][view_x];
		for (int i = 0; i < NUM_TRIANGLES; ++i)
		{
			if (TRIANGLES[i]->color.x == color[0] &&
				TRIANGLES[i]->color.y == color[1] &&
				TRIANGLES[i]->color.z == color[2])
			{
				int num = std::rand() % 4;
				TRIANGLES[i]->material = MATERIALS[num];
			}
			glutPostRedisplay();
		}
	}
}

void
createMenu()
{
	int camera = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Start Rotation", 3);
	glutAddMenuEntry("Stop Rotation", 4);

	glutCreateMenu(processMenuEvents);
	glutAddSubMenu("Camera", camera);
}

int
main(int argc, char **argv)
{
	std::cout << "\n\nAssignment 8 \n\nFeatures:\n\n"
		" - Right Click Menu\n"
		"	- Camera\n"
		"		- Start Rotation\n"
		"		- Stop Rotation\n\n"
		" - Keys\n"
		"	'q' - increases height of camera\n"
		"	'a' - decreases height of camera\n"
		"	'w' - increases radius of camera\n"
		"	's' - decreases radius of camera\n"
		"	'e' - increases speed of camera rotation\n"
		"	'd' - decreases speed of camera rotation\n"
		"	'mouse_clock - changes colors of triangles when clicked"
		"	' ' (SPACE) - reset default view\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Assignment 8");
	glutPositionWindow(0, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);

	createMenu();
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glewExperimental = GL_TRUE;
	glewInit();
	init();
	createFrameBuffer();
	glutMainLoop();
	return 0;
}