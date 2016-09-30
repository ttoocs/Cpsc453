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
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);



//Global variables

GLuint vertex=0;
GLuint fragment=0;
GLuint program=0;

GLuint  vertexBuffer=0;
GLuint  colourBuffer=0;
GLuint  vertexArray=0;

GLFWwindow *window = 0;

void setup_program(){
	
	string vertexSource = LoadSource("vertex.glsl");
    string fragmentSource = LoadSource("fragment.glsl");

    // compile shader source into shader objects
    vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // link shader program
    program = LinkProgram(vertex, fragment);
}

void Render(const GLfloat vertices[][2], int v_size, const GLfloat colours[][3], int c_size, int start, int stop){

	const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;	//Black magic?

	// create an array buffer object for storing our vertices
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, v_size, vertices, GL_STATIC_DRAW);

    // create another one for storing our colours
    glGenBuffers(1, &colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, c_size, colours, GL_STATIC_DRAW);

    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // assocaite the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);

    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	 // clear screen to a dark grey colour
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(program);
    glBindVertexArray(vertexArray);
    glDrawArrays(GL_LINES, start, stop);

    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);



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
	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//Setup the window, with things.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(512, 512, "CPSC 453 Assignment 1", 0, 0);

	if (!window) {
		cout << "Failed to create window, giving up." << endl;
//		end_prog(-2);	
	}
	
	// set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);

	setup_program();
	const GLfloat vertices[][2] = {
        { -0.6, -0.4 },
        {  0.6, -0.4 },
        {  0.0,  0.6 }
    };
    const GLfloat colours[][3] = {
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 }
    };

	Render(vertices,sizeof(vertices),colours,sizeof(colours),0,4);
	glfwSwapBuffers(window);
	
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
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);
    
    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);
    
    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}   
    
// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();
         
    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);

    // try linking the program with given attachments
    glLinkProgram(programObject);
    
    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {   
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }       
        
    return programObject;
}       


