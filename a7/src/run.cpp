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
#include "mesh.h"

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
	vec4 position = vec4(3, 3, 9, 1);
	vec4 at = vec4(0, 0, 0, 1.0);
	vec4 up = vec4(0, 1, 0, 1.0);
	double speed = 0.1;
	double radius = 10;
	double height = 0;
};

struct ControlPoints
{
	const vec3 SELECTED_COLOR = vec3(0.29, 0, 0.5);
	const vec3 DEFAULT_COLOR = vec3(1, 1, 0);
	vec3 points[4][4];
	GLuint shader;
	GLuint vao;
	GLuint vbo;
	vec2 selected = vec2(0, 0);
	void select(int x, int y) {
		this->selected = vec2(x, y);
	}
	vec3 getSelected()
	{
		return this->points[(int)this->selected.x][(int)this->selected.y];
	}
	void setSelected(vec3 point)
	{
		this->points[(int)this->selected.x][(int)this->selected.y] = point;
	}
	ControlPoints(GLuint shader)
	{
		this->shader = shader;
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &this->vbo);
	}
};

struct Axes
{
	GLuint vao;
	GLuint vbo;
	GLuint shader;
	Axes(GLuint shader)
	{
		this->shader = shader;
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &this->vbo);
	}
};

ControlPoints* CTRLPOINTS;
Camera * CAMERA;
Axes * AXES;

GLuint PROGRAM, GOURAUD, PHONG;
int RESOLUTION = 10;

bool IS_ANIMATED = false;
//
clock_t CLOCK = clock();
double THETA = 0;

vec3 LOWER_BOUND = vec3(0, 0, -1.1);
vec3 UPPER_BOUND = vec3(6, 6, 3.9);
vec3 CENTER_POINT = (LOWER_BOUND + UPPER_BOUND) / 2.0;

Material GREEN_PLASTIC = Material(vec3(0.0, 0.0, 0.0), vec3(0.1, 0.35, 0.1), vec3(0.45, 0.55, 0.45), 0.25);

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
loadControlPoints(const char * path)
{
	std::ifstream fin(path);
	std::string line;

	if (!fin)
	{
		std::cout << "Error loading model from '" << path << "'." << std::endl;
		exit(1);
	}
	int col = 0;
	int row = 0;
	while (std::getline(fin, line))
	{
		if (line.length() <= 1)
			continue;
		std::istringstream iss(line);
		std::vector<std::string> tokens{
			std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{} };
		CTRLPOINTS->points[row][col] = vec3((double)std::stof(tokens[0]), (double)std::stof(tokens[1]), (double)std::stof(tokens[2]));
		row++;
		if (row == 4)
		{
			col++;
			row = 0;
		}
	}
}

void
initAxes()
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;

	vertices.push_back(vec3(10, 0, 0));
	vertices.push_back(vec3(-10, 0, 0));

	vertices.push_back(vec3(0, 10, 0));
	vertices.push_back(vec3(0, -10, 0));

	vertices.push_back(vec3(0, 0, 10));
	vertices.push_back(vec3(0, 0, -10));

	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 0));

	glUseProgram(CTRLPOINTS->shader);
	glGenVertexArrays(1, &AXES->vao);
	glGenBuffers(1, &AXES->vbo);
	glBindVertexArray(AXES->vao);
	glBindBuffer(GL_ARRAY_BUFFER, AXES->vbo);

	int v_size = vertices.size() * sizeof(vec3);

	glBufferData(GL_ARRAY_BUFFER, v_size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, v_size, &vertices.front());
	glBufferSubData(GL_ARRAY_BUFFER, v_size, v_size, &colors.front());

	GLuint loc = glGetAttribLocation(CTRLPOINTS->shader, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_colors = glGetAttribLocation(CTRLPOINTS->shader, "vColor");
	glEnableVertexAttribArray(loc_colors);
	glVertexAttribPointer(loc_colors, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_size));
}

void
init(void)
{
	// Load shaders and use the resulting shader program
	CTRLPOINTS = new ControlPoints(InitShader("cpshader.glsl", "fgouraud.glsl"));
	PHONG = InitShader("vphong.glsl", "fphong.glsl");
	PROGRAM = PHONG;
	AXES = new Axes(CTRLPOINTS->shader);
	glClearColor(0, 0, 0, 1.0);
	glUseProgram(PROGRAM);
	
	LIGHTS.push_back(Light(vec3(CENTER_POINT.x, UPPER_BOUND.y + 10, CENTER_POINT.z + 500), vec3(0.1, 0.1, 0.1), vec3(1, 1, 1), vec3(0.1, 0.1, 0.1)));

	load_scene_lighting(PHONG);

	CAMERA = new Camera();

	CAMERA->at = CENTER_POINT;
	//CAMERA->height = CENTER_POINT.y;

	loadControlPoints("control_points.txt");
	initAxes();
}

//----------------------------------------------------------------------------

void
drawControlPoints(mat4 projection, mat4 modelView)
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;

	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			vertices.push_back(CTRLPOINTS->points[row][col]);
			if (row == CTRLPOINTS->selected.x && col == CTRLPOINTS->selected.y)
			{
				colors.push_back(CTRLPOINTS->SELECTED_COLOR);
			}
			else
			{
				colors.push_back(CTRLPOINTS->DEFAULT_COLOR);
			}
		}
	}
	glBindVertexArray(CTRLPOINTS->vao);
	glBindBuffer(GL_ARRAY_BUFFER, CTRLPOINTS->vbo);

	int v_size = vertices.size() * sizeof(vec3);

	glBufferData(GL_ARRAY_BUFFER, v_size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, v_size, &vertices.front());
	glBufferSubData(GL_ARRAY_BUFFER, v_size, v_size, &colors.front());

	GLuint loc = glGetAttribLocation(CTRLPOINTS->shader, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_colors = glGetAttribLocation(CTRLPOINTS->shader, "vColor");
	glEnableVertexAttribArray(loc_colors);
	glVertexAttribPointer(loc_colors, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_size));
	
	GLuint  modelViewLoc = glGetUniformLocation(CTRLPOINTS->shader, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);

	GLuint  projectionLoc = glGetUniformLocation(CTRLPOINTS->shader, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection); 
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, vertices.size());

}

void
drawAxes(mat4 projection, mat4 modelView)
{
	glBindVertexArray(AXES->vao);
	glBindBuffer(GL_ARRAY_BUFFER, AXES->vbo);

	GLuint  modelViewLoc = glGetUniformLocation(CTRLPOINTS->shader, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);

	GLuint  projectionLoc = glGetUniformLocation(CTRLPOINTS->shader, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_LINES, 4, 2);
}

double
getBlendCoff(int i, double u)
{
	if (i == 0)
	{
		return pow((1 - u), 3);
	}
	else if (i == 1)
	{
		return ((3 * u) * pow((1 - u), 2));
	}
	else if (i == 2)
	{
		return ((3 * pow(u, 2)) * (1 - u));
	}
	else if (i == 3)
	{
		return pow(u, 3);
	}
}
vec3
createPoint(double u, double v)
{
	vec3 vertex = vec3(0, 0, 0);
	for (int col = 0; col < 4; ++col)
	{
		double bv = getBlendCoff(col, v);
		for (int row = 0; row < 4; ++row)
		{
			double bu = getBlendCoff(row, u);
			vertex += (bu * bv * CTRLPOINTS->points[row][col]);
		}
	}
	return vertex;
}

std::vector<Vertex>
createMeshVertices()
{
	std::vector<Vertex> vertices;
	for (int i = 0; i <= RESOLUTION; ++i)
	{
		for (int j = 0; j < RESOLUTION; ++j)
		{
			double u = double(i) / (RESOLUTION - 1);
			double v = double(j) / (RESOLUTION - 1);
			vertices.push_back(Vertex(createPoint(u, v), vec3(0, 0, 0)));
		}
	}
	return vertices;
}

std::vector<GLuint>
createTriangles(std::vector<Vertex> & vertices)
{
	std::vector<GLuint> indices;

	for (int col = 0; col < (RESOLUTION - 1); ++col)
	{
		for (int row = 0; row < (RESOLUTION - 1); ++row)
		{
			int bottomLeftIndex = RESOLUTION * col + row;  
			int bottomRightIndex = RESOLUTION * (col + 1) + row;
			int upperLeftIndex = RESOLUTION * col + row + 1;
			int upperRightIndex = RESOLUTION * (col + 1) + row + 1;

			// Create Lower Triangle
			indices.push_back(upperLeftIndex);
			indices.push_back(bottomLeftIndex);
			indices.push_back(bottomRightIndex);
			
			vec3 u = vertices[bottomLeftIndex].position - vertices[upperLeftIndex].position;
			vec3 v = vertices[bottomRightIndex].position - vertices[upperLeftIndex].position;
			vec3 normal = normalize(cross(u, v));
			
			vertices[bottomLeftIndex].normal = normalize(vertices[bottomLeftIndex].normal + normal);
			vertices[bottomRightIndex].normal = normalize(vertices[bottomRightIndex].normal + normal);
			vertices[upperLeftIndex].normal = normalize(vertices[upperLeftIndex].normal + normal);
			
			// Create Upper triangle
			indices.push_back(upperLeftIndex);
			indices.push_back(bottomRightIndex);
			indices.push_back(upperRightIndex);
			
			u = vertices[bottomRightIndex].position - vertices[upperLeftIndex].position;
			v = vertices[upperRightIndex].position - vertices[upperLeftIndex].position;
			normal = normalize(cross(u, v));
			
			vertices[upperLeftIndex].normal = normalize(vertices[upperLeftIndex].normal + normal);
			vertices[bottomRightIndex].normal = normalize(vertices[bottomRightIndex].normal + normal);
			vertices[upperRightIndex].normal = normalize(vertices[upperRightIndex].normal + normal);
		}
	}
	return indices;
}

Mesh
createMesh()
{
	std::vector<Vertex> vertices = createMeshVertices();
	std::vector<GLuint> indices = createTriangles(vertices);
	return Mesh(vertices, indices, GL_TRIANGLES);
}

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);

	glUseProgram(PROGRAM);

	Mesh mesh = createMesh();
	mesh.material = GREEN_PLASTIC;

	load_scene_lighting(PROGRAM);

	vec4 eye = vec4(sin(THETA) * CAMERA->radius, CAMERA->height, cos(THETA) * CAMERA->radius, 1);
	mat4 modelView = LookAt(eye, CAMERA->at, CAMERA->up);

	GLuint  modelViewLoc = glGetUniformLocation(PROGRAM, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);
	GLuint  eyeLoc = glGetUniformLocation(PROGRAM, "eyePosition");
	glUniform3f(eyeLoc, eye.x, eye.y, eye.z);

	mat4 projection = Perspective(45.0f, 1, 0.1, 100);

	GLuint  projectionLoc = glGetUniformLocation(PROGRAM, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
	mesh.Draw(PROGRAM);
	glUseProgram(CTRLPOINTS->shader);
	mat4 modelView2 = LookAt(eye, vec4(0, 0, 0, 1), CAMERA->up);
	drawAxes(projection, modelView2);
	drawControlPoints(projection, modelView);

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
	vec3 point = CTRLPOINTS->getSelected();
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
	case 'u': // up Height
		point.x += 0.1;
		break;
	case 'j': // down Height
		point.x -= 0.1;
		break;
	case 'i': // up Radius
		point.y += 0.1;
		break;
	case 'k': // down Radius
		point.y -= 0.1;
		break;
	case 'o': // up Speed
		point.z += 0.1;
		break;
	case 'l': // down Speed
		point.z -= 0.1;
		break;
	case '1':
		RESOLUTION += 1;
		break;
	case '2':
		RESOLUTION -= 1;
		break;
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
	CTRLPOINTS->setSelected(point);
	std::cout << "SELECTING POINT " << point << std::endl;
	glutPostRedisplay();
}

void
arrowKeys(int key, int x, int y)
{
	int row = CTRLPOINTS->selected.x;
	int col = CTRLPOINTS->selected.y;
	switch (key)
	{
	case GLUT_KEY_UP:
		row += 1;
		if (row == 4)
		{
			row = 0;
		}
		break;
	case GLUT_KEY_DOWN:
		row -= 1;
		if (row == -1)
		{
			row = 3;
		}
		break;
	case GLUT_KEY_LEFT:
		col -= 1;
		if (col == -1)
		{
			col = 3;
		}
		break;
	case GLUT_KEY_RIGHT:
		col += 1;
		if (col == 4)
		{
			col = 0;
		}
		break;
	}
	CTRLPOINTS->select(row, col);
	row = CTRLPOINTS->selected.x;
	col = CTRLPOINTS->selected.y;
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
	std::cout << "\n\nAssignment 7 \n\nFeatures:\n\n"
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
		"	'u' - increases x of selected control point\n"
		"	'j' - decreases x of selected control point\n"
		"	'i' - increases y of selected control point\n"
		"	'k' - decreases y of selected control point\n"
		"	'o' - increases z of selected control point\n"
		"	'l' - decreases z of selected control point\n"
		"	'1' - (one) increases resolution by 1.\n"
		"	'2' - (two) decreases resolution by 1.\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Assignment 7");
	glutPositionWindow(0, 0);
	glutDisplayFunc(display);
	glutSpecialFunc(arrowKeys);
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