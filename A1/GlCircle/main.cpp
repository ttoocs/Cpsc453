//Scott Saunders
//10163541
//Assignment 1, cpsc 453

//Based off:
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

#define PI 3.1415926535f

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors();
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
void regen(int level);

int level=1;
int scene=0;
// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}
// handles keyboard input events, also triggering regeneration of the images.
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		level++;
		regen(level);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		level--;
		if(level <= 0){level=1;}
		regen(level);
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		scene++;
		if(scene >= 3){scene=2;}
		regen(level);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		scene--;
		if(scene <= 0){scene=0;}
		regen(level);
	}
	
}




//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2
vector<vec2> points;
vector<vec3> colors;

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

GLenum rendermode;


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
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec3),
		(void*)0
		);	

	return !CheckGLErrors();		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const vector<vec2>& points, const vector<vec3>& colors)
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
		sizeof(vec3)*colors.size(),
		&colors[0],
		GL_STATIC_DRAW
		);

	return !CheckGLErrors();	
}

//Compile and link shaders, storing the program ID in shader array
bool initShader()
{	
	string vertexSource = LoadSource("vertex.glsl");		//Put vertex file text into string
	string fragmentSource = LoadSource("fragment.glsl");		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	shader[SHADER::LINE] = LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array

	return !CheckGLErrors();
}


//Generate Spiral
// Most of your work will be making new functions like this
// ie: generateSpiral, generateSierpinski, etc...
void generateSpiral(int level){
	
	float scaler = (float) 2*level*PI;

	int	numPoints=150*level;		//Linear Approximation. (Safe)

	//Make sure vectors are empty
	points.clear();
	colors.clear();

	rendermode=GL_LINE_STRIP;

	float u = 0.f;
	float ustep = 1.f/((float)numPoints - 1);		//Size of steps so u = 1 at end of loop

	vec3 startColor(1.f, 0.f, 0.f);		//Initial color
	vec3 endColor(0.f, 0.f, 1.f);			//Final color
	
	for(int i=0; i<numPoints; i++)
	{
		u+=ustep;						//Increment u
		points.push_back(vec2(
			u*cos(scaler*u),		//x coordinate
			-u*sin(scaler*u))		//y coordinate
			);

		colors.push_back(startColor*(1-u) + endColor*u);	//Linearly blend start and end color
	}
}

//Generate Square/Diamond nested.

void generateSquare(int level){

	rendermode=GL_LINES;
	points.clear();
	colors.clear(); 

	vec3 colourS(0.4f,0.f,0.f);
	vec3 colourD(0.f,0.1f,0.4f);

	
	float length = .9f;
		float dynoscale;
	
	for(int i=1; i<= level; i++){
		//DRAW SQUARE (Done in a simple brute-push)	
		points.push_back(vec2(length,length));
		colors.push_back(colourS);
		points.push_back(vec2(-length,length));
		colors.push_back(colourS);	

		points.push_back(vec2(-length,length));
		colors.push_back(colourS);
		points.push_back(vec2(-length,-length));
		colors.push_back(colourS);
	
		points.push_back(vec2(-length,-length));
		colors.push_back(colourS);
		points.push_back(vec2(length,-length));
		colors.push_back(colourS);
	
		points.push_back(vec2(length,-length));
		colors.push_back(colourS);
		points.push_back(vec2(length,length));
		colors.push_back(colourS);

		//DRAW DIAMOND (Done in a simple brute-push)

		points.push_back(vec2(0,length));
		colors.push_back(colourD);
		points.push_back(vec2(length,0));
		colors.push_back(colourD);

		points.push_back(vec2(length,0));
		colors.push_back(colourD);
		points.push_back(vec2(0,-length));
		colors.push_back(colourD);

		points.push_back(vec2(0,-length));
		colors.push_back(colourD);
		points.push_back(vec2(-length,0));
		colors.push_back(colourD);

		points.push_back(vec2(-length,0));
		colors.push_back(colourD);
		points.push_back(vec2(0,length));
		colors.push_back(colourD);
	
		dynoscale = (float) 1/i*0.5f;
		colourS += colourD*dynoscale; //Colour effects for each interation.
		colourD += colourS*dynoscale;

		length /=2 ;					//Shrink the size.
	}
}

void genRecurTri(vec2 pos[], vec3 colour, int reclevel){
	//This could be adapted to use the glDrawElements, to allow for faster&deeper levels, however that is for a future project.
	if(reclevel-- <= 1){
		points.push_back(pos[0]);
		points.push_back(pos[1]);
		points.push_back(pos[2]);		//Push that data if this is a last recursion
		colors.push_back(colour);
		colors.push_back(colour);
		colors.push_back(colour);
		return;
	} //Implicit else via return
	
	vec2 npos[3];
	npos[0]=((pos[0]-pos[1])*0.5f +pos[1]);
	npos[1]=((pos[1]-pos[2])*0.5f +pos[2]); //Generate new verticies
	npos[2]=((pos[2]-pos[0])*0.5f +pos[0]);
	
	vec2 top[] = {npos[1],pos[1],npos[0]};
	genRecurTri(top, colour*.8f,reclevel);
	
	vec2 botl[] = {pos[0],npos[0],npos[2]};
	genRecurTri(botl, colour+vec3(0,0.2f,0),reclevel);		//Mixes colours, verticies, ect, so that the recursed versions are in the right place.

	vec2 botr[] = {npos[2],npos[1],pos[2]};
	genRecurTri(botr, colour+vec3(0.2f,0,0.f),reclevel);
	
}

void generateTri(int level){
	rendermode=GL_TRIANGLES;
	points.clear();
	colors.clear(); 
	//
	// Inital triangle:
	// 						(0,0.43301f)
	// (-0.5f, -0.43301f)					(0.5f,0.43301f)
	//
	
	float scale = 1.8f;
	vec2 pos[] ={
		vec2(-0.5f, -0.43301f)*scale,
		vec2(0,0.43301f)*scale,
		vec2(0.5f,-0.43301f)*scale,
	};

	genRecurTri(pos,				//Initalizes the recursion
		vec3(0.f,0.f,1.f),
		level);

}

void regen(int level){
		switch(scene){
			case 0:
				//Call these two (or equivalents) every time you change geometry
				generateSpiral(level);		//Create geometry - CHANGE THIS FOR DIFFERENT SCENES
				break;
			case 1:
				generateSquare(level);
				break;
			case 2:
				generateTri(level);
				break;
		}
		loadBuffer(points, colors);	//Load geometry into buffers
}


//Initialization
void initGL()
{
	//Only call these once - don't call again every time you change geometry
	generateIDs();		//Create VertexArrayObjects and Vertex Buffer Objects and store their handles
	initShader();		//Create shader and store program ID

	initVAO();			//Describe setup of Vertex Array Objects and Vertex Buffer Objects

}

//Draws buffers to screen
void render()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

	//Don't need to call these on every draw, so long as they don't change
	glUseProgram(shader[SHADER::LINE]);		//Use LINE program
	glBindVertexArray(vao[VAO::LINES]);		//Use the LINES vertex array

	glDrawArrays(
			rendermode,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			0,						//Starting index
			points.size()		//How many vertices
			);
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
    GLFWwindow *window = 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(512, 512, "CPSC 453 Assignment 1", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);

    // query and print out information about our OpenGL environment
    QueryGLVersion();

	initGL();
	regen(level);	
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

bool CheckGLErrors()
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << "GL_OUT_OF_MEMORY" << endl; break;
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
