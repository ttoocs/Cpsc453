//Scott Saunders
//	10163541
// Sept 30, 2016.

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define GLMAJOR_VER 3
#define GLMINOR_VER 3

using namespace std;

//Global variables for GL.
void QueryGLVersion();
bool CheckGLErrors();

//Initalizes various things shared between scenes.
bool initGL(){

	//Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, "vertex.glsl", NULL);
	glCompileShader(vertexShader);

	//Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, "fragment.glsl", NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);	

	return !CheckGLErrors();
}

void end_prog(int code){
	glfwTerminate(); //Terminate this if we need too.
	exit(code);
}


// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}



int main(int argc, char *argv[]){
	if(!glfwInit()){
		cout << "GLFW failed. Goodbye." << endl;
		exit(-1);
	}

	
	GLFWwindow *window 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLMAJOR_VER);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLMINOR_VER);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(512, 512, "CPSC 453 Assignment 1", 0, 0);

	if (!window) {
		cout << "Failed to create window, giving up." << endl;
		end_prog(-2);	
	}
	
	// set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);

	if(!initGL()){
		cout << "Initalizing GL failed" << endl;
		end_prog(-3);
	}

	
}





