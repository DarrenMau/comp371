#include "glew.h"		// include GL Extension Wrangler

#include "glfw3.h"  // include GLFW helper library

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/constants.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<cmath>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cctype>
#include <math.h>  
#include <vector>
#include <fstream>

using namespace std;

#define M_PI        3.14159265358979323846264338327950288   /* pi */
#define DEG_TO_RAD	M_PI/180.0f

GLFWwindow* window = 0x00;

GLuint shader_program = 0;

GLuint view_matrix_id = 0;
GLuint model_matrix_id = 0;
GLuint proj_matrix_id = 0;

// Constant vectors
glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
const glm::vec3 up1(0.0f, 0.0f, 1.0f);
const glm::vec3 left1(1.0f, 0.0f, 0.0f);
const glm::vec3 initialEye(0.0f, 0.0f, 2.0f);


// Screen size
int width = 800;
int height = 800;

// Eye
glm::vec3 eye = initialEye;


///Transformations
glm::mat4 proj_matrix = glm::perspective(45.0f, 1.0f / 1.0f, 0.1f, 100.0f);
glm::mat4 view_matrix(1.0);
glm::mat4 model_matrix;

GLuint VBO, VAO, EBO;
GLenum mode = GL_TRIANGLES;

GLfloat point_size = eye.z;

vector<GLuint> indice;

bool mouseButtonLeftDown = false;
double mousePosY = 0;
double mousePosY_down = 0;
glm::vec3 eyeMouseDown = eye;
float num = 1;
bool back = false;

///Handle the keyboard input
void keyPressed(GLFWwindow *_window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_LEFT:
		//model_matrix = glm::rotate(model_matrix, glm::radians(5.0f), up1);
		center.x = center.x +0.01;
		eye.x = center.x;
		break;
	case GLFW_KEY_RIGHT:
		//model_matrix = glm::rotate(model_matrix, glm::radians(-5.0f), up1);
		center.x = center.x - 0.01;
		eye.x = center.x;
		break;
	case GLFW_KEY_UP:
		//model_matrix = glm::rotate(model_matrix, glm::radians(5.0f), left1);
		center.y = center.y - 0.01;
		eye.y = center.y;
		//eyeMouseDown = eye;
		break;
	case GLFW_KEY_DOWN:
		//model_matrix = glm::rotate(model_matrix, glm::radians(-5.0f), left1);
		center.y = center.y + 0.01;
		eye.y = center.y;
		//eyeMouseDown = eye;
		break;
	case GLFW_KEY_W:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mode = GL_TRIANGLES;
		break;
	case GLFW_KEY_T:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mode = GL_TRIANGLES;
		break;
	case GLFW_KEY_P:
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mode = GL_POINTS;
		break;
	default:
		break;
	}
	return;
}

/// Handle mouse input


void cursorPositionCallback(GLFWwindow* _window, double xpos, double ypos)
{
	mousePosY = ypos;
	
	if (mouseButtonLeftDown)
	{
		cout << xpos << " , " << ypos << endl;
		eye = (float)((mousePosY - mousePosY_down) / (height / 2) + 1) * eyeMouseDown;
	}
}

/**
 *	The function will move the camera close or further when the mouse is click and drag. 
 *
 */
void mouseButtonCallback(GLFWwindow* _window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			mouseButtonLeftDown = true;
			mousePosY_down = mousePosY;
			eyeMouseDown = eye;
		}
		else if (action == GLFW_RELEASE)
		{
			mouseButtonLeftDown = false;
		}
	}
}

/**
 *	The function will handle window resized.
 */
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	proj_matrix = glm::perspective(45.0f, (float)width/height, 0.1f, 10000.0f);
}

bool initialize() {
	/// Initialize GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	/// Create a window of size 640x480 and with title "Lecture 2: First Triangle"
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	window = glfwCreateWindow(width, height, "COMP371: Assignment 1", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwGetWindowSize(window, &width, &height);
	///Register the keyboard callback function: keyPressed(...)
	glfwSetKeyCallback(window, keyPressed);

	///Regster the mouse cursor position callback function: cursorPositionCallback(...)
	glfwSetCursorPosCallback(window, cursorPositionCallback);

	///Regster the mouse button callback function: mouseButtonCallback(...)
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	///Register window resize callback function
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwMakeContextCurrent(window);

	/// Initialize GLEW extension handler
	glewExperimental = GL_TRUE;	///Needed to get the latest version of OpenGL
	glewInit();

	/// Get the current OpenGL version
	const GLubyte* renderer = glGetString(GL_RENDERER); /// Get renderer string
	const GLubyte* version = glGetString(GL_VERSION); /// Version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	/// Enable the depth test i.e. draw a pixel if it's closer to the viewer
	glEnable(GL_DEPTH_TEST); /// Enable depth-testing
	glDepthFunc(GL_LESS);	/// The type of testing i.e. a smaller value as "closer"

	return true;
}

bool cleanUp() {
	glDisableVertexAttribArray(0);
	//Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Close GL context and any other GLFW resources
	glfwTerminate();

	return true;
}

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	//appearing in the vertex shader.
	glBindAttribLocation(ProgramID, 1, "in_Color");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	//The three variables below hold the id of each of the variables in the shader
	//If you read the vertex shader file you'll see that the same variable names are used.
	view_matrix_id = glGetUniformLocation(ProgramID, "view_matrix");
	model_matrix_id = glGetUniformLocation(ProgramID, "model_matrix");
	proj_matrix_id = glGetUniformLocation(ProgramID, "proj_matrix");

	return ProgramID;
}

/**
 *	The function will create a circle from x-axis and z-axis. 
 */
vector<GLfloat> makeCircle(GLfloat x, GLfloat y, GLfloat z, int lineAmount) {
	GLfloat twicePi = M_PI*2;
	//calculate the radius
	GLfloat r = sqrt((pow(x,2) +pow(z,2)));
	vector<GLfloat> vertices;
	for (int i = 0; i <= lineAmount; i++) {
		vertices.push_back(r * cos(i *  twicePi / lineAmount));
		vertices.push_back(y);
		vertices.push_back(r* sin(i * twicePi / lineAmount));
	}
	return vertices;
}

/**
 *	The function will merge 2 vector. 
 */
vector<GLfloat> mergeVector(vector<GLfloat> vector1, vector<GLfloat> vector2) {
	vector<GLfloat> newVec;
	for (int i = 0; i < vector1.size(); i=i+3)
	{
		newVec.push_back(vector1.at(i));
		newVec.push_back(vector1.at(i+1));
		newVec.push_back(vector1.at(i+2));
		newVec.push_back(vector2.at(i));
		newVec.push_back(vector2.at(i + 1));
		newVec.push_back(vector2.at(i + 2));
	}
	return newVec;
}

/**
 *	The function will make the indice.
 */
vector<GLuint> makeIndice(vector<GLfloat> vertices) {
	int i;
	vector<GLuint> indice;
	for (i = 0; i < vertices.size()/3-2; i++) {
		indice.push_back(i);
		indice.push_back(i + 1);
		indice.push_back(i + 2);
	}

	return indice;
}

/**
 *	The function will read one line of point from file.
 */
vector<GLfloat> readLine(std::ifstream* const file)
{
	GLfloat	 x, y, z;
	*file >> x;
	*file >> y;
	*file >> z;
	vector<GLfloat> t = { x,y,z };
	return t;
}

/**
 *	The function will compute the translation sweep.
 */
vector<GLfloat> computeTranslationalSweep(vector<GLfloat> vector1, vector<GLfloat> vector2)
{
	vector<GLfloat> t;
	for (int i = 0; i < vector2.size(); i=i+3)
	{
		for (int j = 0; j < vector1.size(); j=j+3)
		{
			// Iterate through the trajectory curve displacement vectors and translate each profile polyline vertex.
			t.push_back(vector2.at(i) + vector1.at(j));
			t.push_back(vector2.at(i+1) + vector1.at(j+1));
			t.push_back(vector2.at(i+2) + vector1.at(j+2));
		}
	}
	
	return t;
}

/**
 *	The function will make the translation indices.
 */
std::vector<GLuint> makeTranslationIndices(const int p, const int t)
{
	static std::vector<GLuint> indices;
	for (int i = 0; i < p - 1; i++)
	{
		for (int j = 0; j < t - 1; j++)
		{
			const GLuint lowerLeft = i + j	* p;
			const GLuint lowerRight = i + j	* p + 1;
			const GLuint upperLeft = i + (j + 1) * p;
			const GLuint upperRight = i + (j + 1) * p + 1;

			// Lower triangle
			indices.push_back(lowerLeft);
			indices.push_back(lowerRight);
			indices.push_back(upperRight);

			// Upper triangle
			indices.push_back(lowerLeft);
			indices.push_back(upperRight);
			indices.push_back(upperLeft);
		}
	}
	return indices;
}

/**
 *	The function will get the list of first points.
 */
vector<GLfloat> getListTrans(int num_Layer, ifstream* dataFile)
{
	vector<GLfloat> temp;
	vector<GLfloat> vector1;
	for (int i = 0; i < num_Layer; i++)
	{
		temp = readLine(dataFile);
		vector1.push_back(temp.at(0));
		vector1.push_back(temp.at(1));
		vector1.push_back(temp.at(2));
	}
	return vector1;
}

/**
 *	The function will read and make the sweet.
 */
vector<GLfloat> makeSweep(const string filename)
{
	ifstream dataFile(filename); // Open input file
	vector<GLfloat> sweep;
	int f_Line = 0;
	int num_Point = 0;
	int num_Layer;
	if (dataFile.is_open())
	{
		dataFile >> f_Line;
		if (f_Line == 1)
		{
			dataFile >> num_Point;
			dataFile >> num_Layer;
			vector<GLfloat> temp;
			vector<vector<GLfloat>> list_vec;
			for (int i = 0; i < num_Layer; i++)
			{
				temp = readLine(&dataFile);
				list_vec.push_back(makeCircle(temp.at(0), temp.at(1), temp.at(2), num_Point));
			}
			vector<GLfloat> merge;
			for (int i = 0; i < list_vec.size()-1; i++)
			{
				merge = mergeVector(list_vec.at(i), list_vec.at(i+1));
				sweep.insert(sweep.end(), merge.begin(), merge.end());
			}
			indice = makeIndice(sweep);
		}
		else if (f_Line == 0)
		{
			int num_Trans;
			int num_Trans2;
			dataFile >> num_Trans;
			vector<GLfloat> temp;
			vector<GLfloat> vector1;
			vector<GLfloat> vector2;
			vector1 = getListTrans(num_Trans, &dataFile);
			dataFile >> num_Trans2;
			vector2 = getListTrans(num_Trans2, &dataFile);
			sweep = computeTranslationalSweep(vector1, vector2);
			indice = makeTranslationIndices(num_Trans, num_Trans2);
		}
	}
	else
	{
		cerr << "Unable to open file.";
	}
	dataFile.close();
	return sweep;
}

int main() {

	initialize();

	///Load the shaders
	shader_program = loadShaders("COMP371_hw1.vs", "COMP371_hw1.fs");

	// This will identify our vertex buffer
	GLuint vertexbuffer;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	vector<GLfloat> sweep = makeSweep("input_a1.txt");

	glBufferData(GL_ARRAY_BUFFER, sweep.size()*sizeof(GLuint), sweep.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice.size()*sizeof(indice), &indice[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	while (!glfwWindowShouldClose(window)) {
		view_matrix = glm::lookAt(eye, center, up);
		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
		glPointSize(point_size);

		glUseProgram(shader_program);

		//Pass the values of the three matrices to the shaders
		glUniformMatrix4fv(proj_matrix_id, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glBindVertexArray(VAO);
		glDrawElements(mode, indice.size(), GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);

		// update other events like input handling
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	cleanUp();
	return 0;
}