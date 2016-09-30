//Scott Saunders
//	10163541
// Sept 30, 2016.

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>

#include <unistd.h>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define GLMAJOR_VER 3
#define GLMINOR_VER 3

using namespace std;

//Global variables for GL.
void QueryGLVersion();
bool CheckGLErrors();

//Declaring functions defined later.
string LoadSource(const string &filename);


//Initalizes various things shared between scenes.
bool initGL(GLuint * verBuff, GLuint * colBuff){

	
	//Vertex Shader
	string vertexStr = LoadSource("vertex.glsl");
	const GLchar *vertexSource = vertexStr.c_str();
	string fragmentStr = LoadSource("vertex.glsl");
	const GLchar *fragmentSource = fragmentStr.c_str();


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, 0);
	glCompileShader(vertexShader);

	//Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, 0);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	GLint link_ok;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_ok);
	glUseProgram(shaderProgram);	

	if(!link_ok){
		return 1; //Just assume everything happend alright..
	}

	glGenBuffers(1,verBuff);
	glBindBuffer(GL_ARRAY_BUFFER,*verBuff);	//Init glBuffer for verticies

	glGenBuffers(1,colBuff);
	glBindBuffer(GL_ARRAY_BUFFER,*colBuff);	//Init glBuffer for colours.
	
	glClearColor(0,0,0,0);
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


void Render(GLuint *vertexBuffer, GLuint * colourBuffer, GLfloat * verts[], GLfloat * colours[], int start, int stop){
	//Configure the buffers to use the arrays.
	glBufferData(GL_ARRAY_BUFFER, sizeof(*verts), *verts, GL_STATIC_DRAW); //Verts.
	glBufferData(GL_ARRAY_BUFFER, sizeof(*colours), *colours, GL_STATIC_DRAW); //Colours.
	//TODO: Delete these when done.	

    const GLuint VERTEX_INDEX = 0; //Apprently not constants. TODO: Figure out what is actually going on here.
    const GLuint COLOUR_INDEX = 1;

	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	glBindBuffer(GL_ARRAY_BUFFER, *colourBuffer);		
	glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);

	
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_LINES, start, stop);
	
}

void test(GLuint *vertexBuffer, GLuint * colourBuffer){
	GLfloat verts[][2] = {
		{-1,0},{1,0}
	};
	GLfloat colours[][3] = {
		{1,1,1}
	};
	Render(vertexBuffer, colourBuffer, &verts, &colours);
}

int main(int argc, char *argv[]){
	if(!glfwInit()){
		cout << "GLFW failed. Goodbye." << endl;
		exit(-1);
	}

	
	GLFWwindow *window = 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//Setup the window, with things.
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

	GLuint *vertexBuffer;
	GLuint *colourBuffer;
	if(!initGL(vertexBuffer,colourBuffer)){
		cout << "Initalizing GL failed" << endl;
		end_prog(-3);
	}

	test(vertexBuffer, colourBuffer);	
	
	sleep(32);	
}



//Taken verbatim from boilerplate.cpp
// reads a text file with the given name into a string
string LoadSource(const string &filename)
{   
    string source;
    
    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
             << filename << endl;
    }
    
    return source;
}

