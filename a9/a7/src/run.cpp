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
#include <png.h>

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
	vec4 position = vec4(3, 3, 9, 1);
	vec4 at = vec4(0, 0, 0, 1.0);
	vec4 up = vec4(0, 1, 0, 1.0);
	double speed = 0.1;
	double radius = 10;
	double height = 0;
};

struct CPoint
{
	const vec3 SELECTED_COLOR = vec3(0.29, 0, 0.5);
	const vec3 DEFAULT_COLOR = vec3(1, 1, 0);
	vec3 position;
	vec3 color = DEFAULT_COLOR;
	CPoint() :
		position(vec3(0, 0, 0)) {};
	CPoint(vec3 position) :
		position(position) { }
	void Select() { this->color = SELECTED_COLOR; }
	void Unselect() { this->color = DEFAULT_COLOR; }
};

CPoint * CONTROL_POINTS[4][4];
int SELECTED_ROW = 0, SELECTED_COL = 0;

Camera * CAMERA;

vec3 AXES_COLOR = vec3(1, 0, 0);

GLuint PROGRAM, POINTS_VAO, POINTS_VBO, AXES_VAO, AXES_VBO, TEXTURE;
int RESOLUTION = 10;

bool IS_ANIMATED = false;
//
clock_t CLOCK = clock();
double THETA = 0;
float FOV = 45.0;

vec3 LOWER_BOUND = vec3(0, 0, -1.1);
vec3 UPPER_BOUND = vec3(6, 6, 3.9);
vec3 CENTER_POINT = (LOWER_BOUND + UPPER_BOUND) / 2.0;

Material GREEN_PLASTIC = Material(vec4(0.0, 0.0, 0.0, 1), vec4(0.1, 0.35, 0.1, 1), vec4(0.45, 0.55, 0.45, 1), 0.25);

std::vector<Light> LIGHTS;

//--------------------------------------------------------------------------

png_byte * 
png_load(const char * file_name, int * width, int * height)
{
	// This function was originally written by David Grayson for
	// https://github.com/DavidEGrayson/ahrs-visualizer

	png_byte header[8];

	FILE *fp = fopen(file_name, "rb");
	if (fp == 0)
	{
		perror(file_name);
		return 0;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
		fclose(fp);
		return 0;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	if (width) { *width = temp_width; }
	if (height) { *height = temp_height; }

	//printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

	if (bit_depth != 8)
	{
		fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
		return 0;
	}

	GLint format;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_RGB:
		format = GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		format = GL_RGBA;
		break;
	default:
		fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
		return 0;
	}

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes - 1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte) + 15);
	if (image_data == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
	if (row_pointers == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return 0;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	for (unsigned int i = 0; i < temp_height; i++)
	{
		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_pointers);
	fclose(fp);
	return image_data;
}

void
load_texture(png_byte * image_data, int width, int height)
{
	glGenTextures(1, &TEXTURE);
	glBindTexture(GL_TEXTURE_2D, TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);
}

void
load_scene_lighting(GLuint program)
{
	GLint positionLoc, ambientLoc, diffuseLoc, specularLoc;
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
		CONTROL_POINTS[row][col] = new CPoint(vec3((double)std::stof(tokens[0]), (double)std::stof(tokens[1]), (double)std::stof(tokens[2])));
		row++;
		if (row == 4)
		{
			col++;
			row = 0;
		}
	}
	glGenVertexArrays(1, &POINTS_VAO);
	glGenBuffers(1, &POINTS_VBO);
	glBindVertexArray(POINTS_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, POINTS_VBO);

	GLuint loc = glGetAttribLocation(PROGRAM, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	CONTROL_POINTS[SELECTED_ROW][SELECTED_COL]->Select();
}

void
initAxes()
{
	std::vector<vec3> vertices;

	vertices.push_back(vec3(10, 0, 0));
	vertices.push_back(vec3(-10, 0, 0));

	vertices.push_back(vec3(0, 10, 0));
	vertices.push_back(vec3(0, -10, 0));

	vertices.push_back(vec3(0, 0, 10));
	vertices.push_back(vec3(0, 0, -10));

	glUseProgram(PROGRAM);
	glGenVertexArrays(1, &AXES_VAO);
	glBindVertexArray(AXES_VAO);
	glGenBuffers(1, &AXES_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, AXES_VBO);

	int v_size = vertices.size() * sizeof(vec3);

	glBufferData(GL_ARRAY_BUFFER, v_size * 2, NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, v_size, &vertices.front(), GL_STATIC_DRAW);

	GLuint loc = glGetAttribLocation(PROGRAM, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void
init(void)
{
	// Load shaders and use the resulting shader program
	PROGRAM = InitShader("vphong.glsl", "fphong.glsl");
	glClearColor(0, 0, 0, 1.0);
	glUseProgram(PROGRAM);
	
	int height = 256;
	int width = 256;

	png_byte* image_data = png_load("texture.png", &width, &height);
	load_texture(image_data, width, height);
	free(image_data);

	LIGHTS.push_back(Light(vec3(0, 0, 0), vec3(0.2, 0.2, 0.2), vec3(0.6, 0.6, 0.6), vec3(0.05, 0.05, 0.05)));

	load_scene_lighting(PROGRAM);

	CAMERA = new Camera();

	CAMERA->at = CENTER_POINT;

	loadControlPoints("control_points.txt");
	initAxes();
}

//----------------------------------------------------------------------------

void
updateControlPoints()
{
	std::vector<vec3> vertices;

	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			vertices.push_back(CONTROL_POINTS[row][col]->position);
		}
	}
	glBindVertexArray(POINTS_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, POINTS_VBO);
	int v_size = vertices.size() * sizeof(vec3);
	glBufferData(GL_ARRAY_BUFFER, v_size, &vertices.front(), GL_STATIC_DRAW);
}

void
drawControlPoints()
{
	updateControlPoints();
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);

	GLuint  colorFlagLoc = glGetUniformLocation(PROGRAM, "const_color");
	GLuint  colorLoc = glGetUniformLocation(PROGRAM, "vColor");
	glUniform1f(colorFlagLoc, 1.0f);

	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			glUniform3fv(colorLoc, 1, (GLfloat*)&CONTROL_POINTS[row][col]->color);
			glDrawArrays(GL_POINTS, (col * 4 + row), 1);
		}
	}
}

void
drawAxes()
{
	glBindVertexArray(AXES_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, AXES_VBO);

	GLuint  modelLoc = glGetUniformLocation(PROGRAM, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, mat4());

	GLuint  colorFlagLoc = glGetUniformLocation(PROGRAM, "const_color");
	GLuint  colorLoc = glGetUniformLocation(PROGRAM, "vColor");
	glUniform1f(colorFlagLoc, 1.0f);
	glUniform3fv(colorLoc, 1, (GLfloat*)&AXES_COLOR);

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
			vertex += (bu * bv * CONTROL_POINTS[row][col]->position);
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
			vertices.push_back(Vertex(createPoint(u, v), vec3(0, 0, 0), vec2(u, v)));
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
	
	vec4 eye = vec4(sin(THETA) * CAMERA->radius, CAMERA->height, cos(THETA) * CAMERA->radius, 1);
	mat4 view = LookAt(eye, CAMERA->at, CAMERA->up);

	LIGHTS[0].position = vec3(eye.x, eye.y, eye.z);

	load_scene_lighting(PROGRAM);

	glUniform1i(glGetUniformLocation(PROGRAM, "texture"), 0);

	GLuint  modelLoc = glGetUniformLocation(PROGRAM, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, mat4());

	GLuint  modelViewLoc = glGetUniformLocation(PROGRAM, "view");
	glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, view);

	GLuint  eyeLoc = glGetUniformLocation(PROGRAM, "eyePosition");
	glUniform3f(eyeLoc, eye.x, eye.y, eye.z);

	mat4 projection = Perspective(FOV, 1, 0.1, 100);

	GLuint  projectionLoc = glGetUniformLocation(PROGRAM, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
	 
	GLuint  colorFlagLoc = glGetUniformLocation(PROGRAM, "const_color");
	glUniform1f(colorFlagLoc, 0.0f);

	mesh.Draw(PROGRAM);

	drawAxes();
	drawControlPoints();

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
	vec3* point = &CONTROL_POINTS[SELECTED_ROW][SELECTED_COL]->position;
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
	case 'u': // up Height
		point->x += 0.1;
		break;
	case 'j': // down Height
		point->x -= 0.1;
		break;
	case 'i': // up Radius
		point->y += 0.1;
		break;
	case 'k': // down Radius
		point->y -= 0.1;
		break;
	case 'o': // up Speed
		point->z += 0.1;
		break;
	case 'l': // down Speed
		point->z -= 0.1;
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
	glutPostRedisplay();
}

void
arrowKeys(int key, int x, int y)
{
	CONTROL_POINTS[SELECTED_ROW][SELECTED_COL]->Unselect();
	switch (key)
	{
	case GLUT_KEY_UP:
		SELECTED_ROW += 1;
		if (SELECTED_ROW == 4)
		{
			SELECTED_ROW = 0;
		}
		break;
	case GLUT_KEY_DOWN:
		SELECTED_ROW -= 1;
		if (SELECTED_ROW == -1)
		{
			SELECTED_ROW = 3;
		}
		break;
	case GLUT_KEY_LEFT:
		SELECTED_COL -= 1;
		if (SELECTED_COL == -1)
		{
			SELECTED_COL = 3;
		}
		break;
	case GLUT_KEY_RIGHT:
		SELECTED_COL += 1;
		if (SELECTED_COL == 4)
		{
			SELECTED_COL = 0;
		}
		break;
	}
	CONTROL_POINTS[SELECTED_ROW][SELECTED_COL]->Select();
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