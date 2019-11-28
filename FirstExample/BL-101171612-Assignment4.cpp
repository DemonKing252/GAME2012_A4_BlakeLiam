//***************************************************************************
// BL-101171612-Assignment4.cpp by Liam Blake (C) 2018 All Rights Reserved.
//
// Assignment 4 submission.
//
// Description:
// Press F5 to see a rubix cube with lighting effects!	
//
//*****************************************************************************
using namespace std;
#include <iostream>
#include "stdlib.h"
#include "time.h"
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include <SOIL.h>
#include <vector>
#include "glm\gtc\matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
int numSquares;
GLuint cube_vao, bonus_vao, platform_vao, ibo, points_vbo, cube_colours_vbo, pyramid_colours_vbo, platcolors_vbo, matrixID, modelID, projID, viewID, normals_vbo;
GLfloat* normals;
float rotAngle = 0.0f;

// Horizontal and vertical ortho offsets.
float osH = 0.0f, osV = 0.0f, scrollSpd = 0.25f;
int deltaTime, currentTime, lastTime = 0;
glm::mat4 model, view, projection, MVP;
GLfloat cameraX = 0, cameraY = 0, cameraZ = -5;
#define ABSOLUTE_MAX_DIMENSIONS 100		// Don't let the player enter more than 100.
int rows, cols;
const int ROWS = ABSOLUTE_MAX_DIMENSIONS;
const int COLS = ABSOLUTE_MAX_DIMENSIONS;

// So neat and organized : )
GLshort* plane_indicies;// [16 * (ABSOLUTE_MAX_DIMENSIONS * ABSOLUTE_MAX_DIMENSIONS)];
GLfloat* plane_vertices;// [12 * (ABSOLUTE_MAX_DIMENSIONS * ABSOLUTE_MAX_DIMENSIONS)];
GLfloat* textureCoordinates;

void CalculateNormals(GLshort* indices, unsigned short intdiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, float * normals)
{
	for (int i = 0; i < intdiceCount / 3; i += 3)
	{
		unsigned int in0 = indices[i + 0];
		unsigned int in1 = indices[i + 1];
		unsigned int in2 = indices[i + 2];

		glm::vec3 v1(vertices[in1 * 3 + 0] - vertices[in0 * 3 + 0], vertices[in1 * 3 + 1] - vertices[in0 * 3 + 1], vertices[in1 * 3 + 2] - vertices[in0 * 3 + 2]);
		glm::vec3 v2(vertices[in2 * 3 + 0] - vertices[in0 * 3 + 0], vertices[in2 * 3 + 1] - vertices[in0 * 3 + 1], vertices[in2 * 3 + 2] - vertices[in0 * 3 + 2]);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		normals[in0 + 0] = normal.x;
		normals[in0 + 1] = normal.y;
		normals[in0 + 2] = normal.z;

		normals[in1 + 0] = normal.x;
		normals[in1 + 1] = normal.y;
		normals[in1 + 2] = normal.z;

		normals[in2 + 0] = normal.x;
		normals[in2 + 1] = normal.y;
		normals[in2 + 2] = normal.z;

	}

}

GLfloat proj = 10.0f;
GLfloat mouseX, mouseY;
GLfloat colours[] = {
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,

	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,

	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f

};

GLint width, height, width2, height2;
GLuint cube_tex = 0, pyramid_tex = 0;
GLuint cube_tex_vbo = 0, pyramid_tex_vbo;
GLfloat colours2[] = {
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,
	0.5f, 0.5f, 1.0f,

	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,
	0.5f, 1.0f, 0.0f,

	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f
};
// Light variables
GLuint program;
//glm::vec3 ambientColour = glm::vec3(1.0f, 1.0f, 1.0f);
//GLfloat ambientStrength = 1.0f;
//glm::vec3 lightDirection = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 diffuseColour = glm::vec3(1.0f, 0.5f, 1.0f);
//GLfloat diffuseStrength = 1.0f;
glm::vec3 currentLightPos = glm::vec3(0.0f, 0.0f, 5.0f);

ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
};
unsigned char* rubicksCubeTexture;
unsigned char* pyramidTexture;
struct Light
{
	GLuint colorHandle;
	GLuint posHandle;
	GLuint strengthHandle;
	GLuint falloffStartHandle;
	GLuint falloffEndHandle;

};
Light pointLights[2];
void initLights()
{
	pointLights[0].colorHandle = glGetUniformLocation(program, "pointLights[0].Color");

	//if (pointLights[0].colorHandle == NULL) cout << "Problem!" << endl;

	pointLights[0].posHandle = glGetUniformLocation(program, "pointLights[0].Position");
	//if (pointLights[0].posHandle == NULL) cout << "Problem!" << endl;
	pointLights[0].strengthHandle = glGetUniformLocation(program, "pointLights[0].Strength");
	//if (pointLights[0].strengthHandle == NULL) cout << "Problem!" << endl;
	pointLights[0].falloffStartHandle = glGetUniformLocation(program, "pointLights[0].falloffStart");
	//if (pointLights[0].falloffStartHandle == NULL) cout << "Problem!" << endl;
	pointLights[0].falloffEndHandle = glGetUniformLocation(program, "pointLights[0].falloffEnd");
	//if (pointLights[0].falloffEndHandle == NULL) cout << "Problem!" << endl;

	pointLights[1].colorHandle = glGetUniformLocation(program, "pointLights[1].Color");
	pointLights[1].posHandle = glGetUniformLocation(program, "pointLights[1].Position");
	pointLights[1].strengthHandle = glGetUniformLocation(program, "pointLights[1].Strength");
	pointLights[1].falloffStartHandle = glGetUniformLocation(program, "pointLights[1].falloffStart");
	pointLights[1].falloffEndHandle = glGetUniformLocation(program, "pointLights[1].falloffEnd");

	glUniform3fv(pointLights[0].colorHandle, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	//currentLightPos = glm::vec3(5, 5, 5);
	glUniform3fv(pointLights[0].posHandle, 1, glm::value_ptr(currentLightPos));
	glUniform1f(pointLights[0].strengthHandle, 1.0f);
	glUniform1f(pointLights[0].falloffStartHandle, 1.0f);
	glUniform1f(pointLights[0].falloffEndHandle, 50.0f);

	glUniform3fv(pointLights[1].colorHandle, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform3fv(pointLights[1].posHandle, 1, glm::value_ptr(glm::vec3(-2.0f, 4.0f, 4.0f)));
	glUniform1f(pointLights[1].strengthHandle, 1.0f);
	glUniform1f(pointLights[1].falloffStartHandle, 1.0f);
	glUniform1f(pointLights[1].falloffEndHandle, 50.0f);

}

void init(void)
{
	//Loading and compiling shaders
	program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up	

	matrixID = glGetUniformLocation(program, "mvp");
	modelID = glGetUniformLocation(program, "M");
	projID = glGetUniformLocation(program, "projection");
	viewID = glGetUniformLocation(program, "V");
	// Setting ambient light

	//glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	//glUniform3f(glGetUniformLocation(program, "ambientColour"), ambientColour.x, ambientColour.y, ambientColour.z);
	//glUniform1f(glGetUniformLocation(program, "ambientStrength"), ambientStrength);

	//calcAverageNormals(cube_indices, 24, cube_vertices, 176, 8, 5);
	// start
	cube_vao = 0;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 16 * rows*cols, plane_indicies, GL_STATIC_DRAW);


	points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * rows*cols, plane_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//cube_colours_vbo = 0;
	//glGenBuffers(1, &cube_colours_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, cube_colours_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(colours2), colours2, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(1);


	rubicksCubeTexture = SOIL_load_image("bonusTexture.png", &width, &height, 0, SOIL_LOAD_RGB);


	glGenTextures(1, &cube_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rubicksCubeTexture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenBuffers(1, &cube_tex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_tex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * rows*cols, textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	//glGenBuffers(1, &cube_tex_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, cube_tex_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8*rows*cols, textureCoordinates, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);

	normals_vbo = 0;
	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(12 * rows*cols), normals, GL_STATIC_DRAW); // This will not work
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);
}

//---------------------------------------------------------------------
//x
// transformModel
//
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	model = projection * view * Model;
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);

	MVP = projection * view * model;
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	initLights();

	view = glm::lookAt(// 25 37.5
		glm::vec3(cameraX, cameraY, cameraZ + 10.0),		// Camera pos in World Space
		glm::vec3(0, 0, 0),	// This is necessary, we need to move the camera origin with the camera pos
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);
	projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(53.0f / 255.0f, 81.0f / 255.0f, 92.0f / 255.0f, 1.0f);

	// Change vaos
	glBindVertexArray(cube_vao);
	glBindTexture(GL_TEXTURE_2D, cube_tex);

	static GLfloat angle = 0.0f;
	// Draw the Plane (up to 100 by 100)
	transformObject(1.0f, Y_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_QUADS, 12 * (rows * cols), GL_UNSIGNED_SHORT, 0);

	//transformObject(1.0f, Y_AXIS, 90.0f, glm::vec3(-1.0f, 0.0f, -1.0f));
	//glDrawElements(GL_QUADS, 12 * (rows * cols), GL_UNSIGNED_SHORT, 0);

	glutSwapBuffers(); // Instead of double buffering.
}

void idle()
{
	//glutPostRedisplay();
}

void timer(int id)
{
	glutPostRedisplay();
	glutTimerFunc(33, timer, 0);
}

void keyDown(unsigned char key, int x, int y)
{
	// Orthographic.
	switch (key)
	{
	case 's':
		currentLightPos.z += 0.5f;
		cameraZ += 0.5f;
		break;
	case 'w':
		currentLightPos.z -= 0.5f;
		cameraZ -= 0.5f;
		break;
	case 'd':
		currentLightPos.x += 0.5f;
		cameraX += 0.5f;
		break;
	case 'a':
		currentLightPos.x -= 0.5f;
		cameraX -= 0.5f;
		break;
	case 'i':
		currentLightPos.y += 0.25f;
		break;
	case 'j':
		currentLightPos.x -= 0.25f;
		break;
	case 'l':
		currentLightPos.x += 0.25f;
		//rotAngle -= 5.0f;
		break;
	case 'k':
		currentLightPos.y -= 0.25f;
		//rotAngle += 5.0f;
		break;
	case 'r':
		cameraY += 0.5f;
		break;
	case 'f':
		cameraY -= 0.5f;
		break;
	}
}

void keyUp(unsigned char key, int x, int y)
{
	// Empty for now
}

void mouseMove(int x, int y)
{
	//cout << "Mouse pos: " << x << "," << y << endl;
	mouseX = x - 512;
	mouseY = y - 512;
}

void mouseDown(int btn, int state, int x, int y)
{
	cout << "Clicked: " << (btn == 0 ? "left " : "right ") << (state == 0 ? "down " : "up ") <<
		"at " << x << "," << y << endl;
}
void CreateDivisions()
{
	cout << "[=================================================================]" << endl;
	cout << "Please enter the number of rows in the plane (Max is 100): ";
	cin >> rows;
	cout << "Please enter the number of columns in the plane (Max is 100): ";
	cin >> cols;

	if (rows > 100)
		rows = 100;
	if (cols > 100)
		cols = 100;
	// Set the indices, textureCoordinates, and vertices
	plane_indicies = new GLshort[12 * (cols * rows)];
	plane_vertices = new GLfloat[12 * (cols * rows)];
	textureCoordinates = new GLfloat[8 * (rows * cols)];
	normals = new GLfloat[12 * (rows * cols)]{ 0 };
	int index = 0;
	float x = 0;
	float y = 0;
	while (index < (12 * (rows * cols)))
	{
		plane_vertices[index] = (0.5) - x * 1 / cols;
		plane_vertices[index + 1] = (0.5) - y * 1 / rows;    // Top-Left Corner
		plane_vertices[index + 2] = (0.5) - 0.0f;

		plane_vertices[index + 3] = (0.5) - (1 + x) * 1 / cols;
		plane_vertices[index + 4] = (0.5) - y * 1 / rows;	// Top-Right Corner
		plane_vertices[index + 5] = (0.5) - 0.0f;

		plane_vertices[index + 6] = (0.5) - (1 + x) * 1 / cols;
		plane_vertices[index + 7] = (0.5) - (1 + y) * 1 / rows;	// Bottom-Right Corner
		plane_vertices[index + 8] = (0.5) - 0.0f;

		plane_vertices[index + 9] = (0.5) - x * 1 / cols;
		plane_vertices[index + 10] = (0.5) - (1 + y) * 1 / rows;	// Bottom-Left Corner
		plane_vertices[index + 11] = (0.5) - 0.0f;


		x += 1.0f;
		if (x == cols)
		{
			x = 0.0f;
			y += 1.0f;
		}
		index += 12;
	}
	index = 0;
	y = 0;
	x = 0;
	while (index < 8 * (rows * cols))
	{
		//textureCoordinates[index] = 0.0f;
		//textureCoordinates[index + 1] = 0.0f;
		//
		//textureCoordinates[index + 2] = 1.0f;
		//textureCoordinates[index + 3] = 0.0f;
		//
		//textureCoordinates[index + 4] = 1.0f;
		//textureCoordinates[index + 5] = 1.0f;
		//
		//textureCoordinates[index + 6] = 0.0f;
		//textureCoordinates[index + 7] = 1.0f;
		// UNCOMMENT THIS, AND UNCOMMENT THE OTHER BLOCK FOR PROVE THAT IT WORKS
		textureCoordinates[index] = (0.5) - x * (1 / (float)cols);
		textureCoordinates[index + 1] = (0.5) - y * (1 / (float)rows);

		textureCoordinates[index + 2] = (0.5) - (1 + x) * (1 / (float)cols);
		textureCoordinates[index + 3] = (0.5) - y * (1 / (float)rows);

		textureCoordinates[index + 4] = (0.5) - (1 + x) * (1 / (float)cols);
		textureCoordinates[index + 5] = (0.5) - (1 + y) * (1 / (float)rows);

		textureCoordinates[index + 6] = (0.5) - x * (1 / (float)cols);
		textureCoordinates[index + 7] = (0.5) - (1 + y) * (1 / (float)rows);
		x += 1.0f;
		if (x == cols)
		{
			x = 0.0f;
			y += 1.0f;
		}
		index += 8;
	}
	x = 0.0f;
	y = 0.0f;
	for (int i = 0; i < (12 * (rows * cols)); i++)
	{
		plane_indicies[i] = i;
	}
	CalculateNormals(plane_indicies, 12 * rows * cols, plane_vertices, 12 * rows * cols, 5, normals);
}
//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{

	CreateDivisions();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Blake, Liam, 101171612");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();
	// Set all our glut functions.
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutTimerFunc(33, timer, 0);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutPassiveMotionFunc(mouseMove); // or...
	//glutMotionFunc(mouseMove); // Requires click to register.
	glutMainLoop();
}