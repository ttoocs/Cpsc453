// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include "glm/glm.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define PI 3.14159265359

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

vec2 mousePos;
GLFWwindow* window = 0;

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}




//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2
vector<vec2> points;
vector<vec2> uvs;

//Structs are simply acting as namespaces
//Access the values like so: VAO::LINES
struct VAO{
	enum {LINES=0, COUNT};		//Enumeration assigns each name a value going up
										//LINES=0, COUNT=1
};

struct VBO{
	enum {POINTS=0, COLOR, COUNT};	//POINTS=0, COLOR=1, COUNT=2
};

struct SHADER{
	enum {LINE=0, COUNT};		//LINE=0, COUNT=1
};

GLuint vbo [VBO::COUNT];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao [VAO::COUNT];		//Array which stores Vertex Array Object handles
GLuint shader [SHADER::COUNT];		//Array which stores shader program handles


//Gets handles from OpenGL
void generateIDs()
{
	glGenVertexArrays(VAO::COUNT, vao);		//Tells OpenGL to create VAO::COUNT many
														// Vertex Array Objects, and store their
														// handles in vao array
	glGenBuffers(VBO::COUNT, vbo);		//Tells OpenGL to create VBO::COUNT many
													//Vertex Buffer Objects and store their
													//handles in vbo array
}

//Clean up IDs when you're done using them
void deleteIDs()
{
	for(int i=0; i<SHADER::COUNT; i++)
	{
		glDeleteProgram(shader[i]);
	}
	
	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);	
}


//Describe the setup of the Vertex Array Object
bool initVAO()
{
	glBindVertexArray(vao[VAO::LINES]);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO::POINTS] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		2,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec2),	//Stride
		(void*)0			//Offset
		);
	
	glEnableVertexAttribArray(1);		//Tell opengl you're using layout attribute 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::COLOR]);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec2),
		(void*)0
		);	

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const vector<vec2>& points, const vector<vec2>& colors)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec2)*points.size(),	//Size of data in array (in bytes)
		&points[0],							//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
												//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::COLOR]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec2)*uvs.size(),
		&uvs[0],
		GL_STATIC_DRAW
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
bool initShader()
{	
	string vertexSource = LoadSource("vertex.glsl");		//Put vertex file text into string
	string fragmentSource = LoadSource("fragment.glsl");		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	shader[SHADER::LINE] = LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array

	return !CheckGLErrors("initShader");
}

//For reference:
//	https://open.gl/textures
GLuint createTexture(const char* filename)
{
	int components;
	GLuint texID;
	int tWidth, tHeight;

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &tWidth, &tHeight, &components, 0);
	
	if(data != NULL)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		if(components==3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(components==4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Clean up
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texID;
	} 
	
	return 0;	//Error
}

//Use program before loading texture
//	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
bool loadTexture(GLuint texID, GLuint texUnit, GLuint program, const char* uniformName)
{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
	
	GLuint uniformLocation = glGetUniformLocation(program, uniformName);
	glUniform1i(uniformLocation, 0);
		
	return !CheckGLErrors("loadTexture");
}

void generateSquare(float width)
{
	vec2 p00 = vec2(
		-width*0.5f,
		width*0.5f);
	vec2 uv00 = vec2(
		0.f,
		0.f);

	vec2 p01 = vec2(
		width*0.5f,
		width*0.5f);
	vec2 uv01 = vec2(
		1.f,
		0.f);

	vec2 p10 = vec2(
		-width*0.5f,
		-width*0.5f);
	vec2 uv10 = vec2(
		0.f,
		1.f);
	
	vec2 p11 = vec2(
		width*0.5f,
		-width*0.5f);
	vec2 uv11 = vec2(
		1.f,
		1.f);

	//Triangle 1
	points.push_back(p00);
	points.push_back(p10);
	points.push_back(p01);
	
	uvs.push_back(uv00);
	uvs.push_back(uv10);
	uvs.push_back(uv01);

	//Triangle 2
	points.push_back(p11);
	points.push_back(p01);
	points.push_back(p10);

	uvs.push_back(uv11);
	uvs.push_back(uv01);
	uvs.push_back(uv10);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	
}

bool loadUniforms()
{
	//Get mouse pixel coordinates
/*	double x, y;
	glfwGetCursorPos(window, &x, &y);

	int vp [4];
	glGetIntegerv(GL_VIEWPORT, vp);		//Get viewport dimensions- {x offset, y offset, width, height}


	//For more uniforms - look up https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
	//Use the glUniform call that fits the type you're loading
	GLint uniformLocation = glGetUniformLocation(shader[SHADER::LINE], "mousePos");
	glUniform2f(uniformLocation,
		(float)(x/(double)vp[2]), (float)(y/(double)vp[3]));	//Normalize coordinates between 0 and 1

*/
	return !CheckGLErrors("loadUniforms");
}

//Initialization
void initGL()
{
	//Only call these once - don't call again every time you change geometry
	generateIDs();		//Create VertexArrayObjects and Vertex Buffer Objects and store their handles
	initShader();		//Create shader and store program ID

	initVAO();			//Describe setup of Vertex Array Objects and Vertex Buffer Objects

	//Call these two (or equivalents) every time you change geometry
	generateSquare(2.f);
	loadBuffer(points, uvs);	//Load geometry into buffers
}

//Draws buffers to screen
void render()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

	//Don't need to call these on every draw, so long as they don't change
	glUseProgram(shader[SHADER::LINE]);		//Use LINE program
	glBindVertexArray(vao[VAO::LINES]);		//Use the LINES vertex array

	loadUniforms();

	glDrawArrays(
			GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			0,						//Starting index
			points.size()		//How many vertices
			);

	CheckGLErrors("render");
}




// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{   
    // initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
        return -1;
    }
    glfwSetErrorCallback(ErrorCallback);

    // attempt to create a window with an OpenGL 4.1 core profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Boilerplate", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    // query and print out information about our OpenGL environment
    QueryGLVersion();

	initGL();

    // run an event-triggered main loop
    while (!glfwWindowShouldClose(window))
    {
        // call function to draw our scene
        render();

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

        // sleep until next event before drawing again
        glfwWaitEvents();
	}

	// clean up allocated resources before exit
   deleteIDs();
	glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
    // query opengl version and renderer information
    string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL [ " << version << " ] "
         << "with GLSL [ " << glslver << " ] "
         << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
        default:
            cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

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

// creates and returns a shader object compiled from the given source
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


// ==========================================================================
