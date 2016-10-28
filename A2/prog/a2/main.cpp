// Scott Saunders, 10163541
// Oct 9, 2016


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
#include <algorithm>
#include <string>
#include <iterator>

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <glm/glm.hpp>


#define PI 3.1415926535f
#define WINDOW_X 512
#define WINDOW_Y 512

#define PICTURES {"image1-mandrill.png","image2-uclogo.png","image3-aerial.jpg","image4-thirsk.jpg","image5-pattern.png","test.png","pt_hero.jpg"}

using namespace std;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

float c = 0;
GLfloat zoom = 1.f;
GLfloat trans[2][2] = {{1,0},{0,1}}; 	//2D transformation Matrix
GLfloat offset[2] = {0,0};				//2D offset vector
GLfloat theta = 0;

GLfloat mouseX;
GLfloat mouseY;
GLfloat ImouseX;	//Various mouse defines.
GLfloat ImouseY;
GLfloat DmouseX;
GLfloat DmouseY;
bool Mdown = false;

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

bool reload_pic=false;
int Picnum=0;
char * Pictures[]=PICTURES;

void mouseoffset(){	
	DmouseX = ImouseX-mouseX;
	DmouseY = ImouseY-mouseY;
	
	offset[0] -= DmouseX;
	offset[1] += DmouseY;

}

void recalc_trans(GLfloat newtheta){
	theta += newtheta;
	trans[0][0]=cos(theta);
	trans[0][1]=-sin(theta);
	trans[1][0]=sin(theta);
	trans[1][1]=cos(theta);

	GLfloat newoff[2] = {
		offset[0]*cos(newtheta)+offset[1]*sin(newtheta), 
		-offset[0]*sin(newtheta)+offset[1]*cos(newtheta)};
	offset[0]=newoff[0];
	offset[1]=newoff[1];
}

//Structures for various data-types.
struct fData {
	int bw_mode;
	int conv_mode;
	fData() : bw_mode(0), conv_mode(0)
	{}
};
fData fragData;

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  fragment;
	GLuint  program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader() : vertex(0), fragment(0), program(0)
	{}
};

struct MyTexture
{
	GLuint textureID;
	GLuint target;
	int width;
	int height;

	// initialize object names to zero (OpenGL reserved value)
	MyTexture() : textureID(0), target(0), width(0), height(0)
	{}
};


// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("vertex.glsl");
	string fragmentSource = LoadSource("fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing textures


bool InitializeTexture(MyGeometry *geometry, MyTexture* texture, const char* filename, GLuint target = GL_TEXTURE_2D)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename, &texture->width, &texture->height, &numComponents, 0);
	if (data != nullptr)
	{
		texture->target = target;
		glGenTextures(1, &texture->textureID);
		glBindTexture(texture->target, texture->textureID);
		GLuint format = numComponents == 3 ? GL_RGB : GL_RGBA;
		cout << numComponents << endl;
		glTexImage2D(texture->target, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);

		// Note: Only wrapping modes supported for GL_TEXTURE_RECTANGLE when defining
		// GL_TEXTURE_WRAP are GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

		GLfloat ratio = ( ((GLfloat) texture->width) / ((GLfloat)texture->height));
		if(ratio >= 1){
			const GLfloat vertices[][2] = {
				{-1.f,	-1/ratio},
				{-1.f,	1/ratio},
				{1.f,	-1/ratio},
				{1.f,	1/ratio}
			};
			glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		}else{
			const GLfloat vertices[][2] = {
				{-ratio, 	-1.f},
				{-ratio,	1.f},
				{ratio,	-1.f},
				{ratio,	1.f}
			};
			glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		}

		const GLfloat textureCoords[][2] = {
			{0,						0},
//			{0, 		texture->width},
			{0,	texture->height},
//			{texture->height,	 	0},
			{texture->width,	0},
//			{texture->height, texture->width}	
			{texture->width,	texture->height}
			};
		glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

		// Clean up
		glBindTexture(texture->target, 0);
		stbi_image_free(data);
		return !CheckGLErrors();
	}
	return true; //error
}

// deallocate texture-related objects
void DestroyTexture(MyTexture *texture)
{
	glBindTexture(texture->target, 0);
	glDeleteTextures(1, &texture->textureID);
}

void SaveImage(const char* filename, int width, int height, unsigned char *data, int numComponents = 3, int stride = 0)
{
	if (!stbi_write_png(filename, width, height, numComponents, data, stride))
		cout << "Unable to save image: " << filename << endl;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data


// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry)
{
	// three vertex positions and assocated colours of a triangle
	const GLfloat vertices[][2] = {
		{ -1.f, +1.f },
		{ 1.f,  1.f },			//Not really used save for initalization
		{ -1.f, -1.f },
		{ 1.f,	-1.f}
	};

	const GLfloat textureCoords[][2] = {
		{0.f, 0.f},
		{0.f, 512.f},			//Not really used save for initalization
		{512.f, 0.f},
		{512.f,512.f}
	};

	const GLfloat colours[][3] = {
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },	//Not used.
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }
	};
	geometry->elementCount = 4;

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;
	const GLuint TEXTURE_INDEX = 2;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//
	glGenBuffers(1, &geometry->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	// Tell openGL how the data is formatted
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glVertexAttribPointer(TEXTURE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXTURE_INDEX);

	// assocaite the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyTexture* texture, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glBindTexture(texture->target, texture->textureID);

	GLint uniTransform = glGetUniformLocation(shader->program, "transform");
	glUniformMatrix2fv(uniTransform, 1, GL_FALSE, *trans);

	GLint uniZoom = glGetUniformLocation(shader->program, "zoom");
	glUniform1f(uniZoom, zoom);

	GLint uniOffset = glGetUniformLocation(shader->program, "offset");
	glUniform2fv(uniOffset, 1, offset);

	GLint uniData = glGetUniformLocation(shader->program, "fragData");
	glUniform1i(uniData+1, fragData.bw_mode);
	glUniform1i(uniData+2, fragData.conv_mode); 

	glDrawArrays(GL_TRIANGLE_STRIP, 0, geometry->elementCount);
//	printf("hiw3432\n");
	CheckGLErrors();
//	printf("ho523\n");

	// reset state to default (no shader or geometry bound)
	glBindTexture(texture->target, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL error
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Mdown=true;
//		Moffset[0] = offset[0];
//		Moffset[1] = offset[1];
		ImouseX = mouseX;
		ImouseY = mouseY;
	}
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		Mdown=false;
//		mouseoffset();
	}
	
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX= (xpos/WINDOW_X*2 -1.f)/zoom;
	mouseY= (ypos/WINDOW_Y*2 -1.f)/zoom;
	if(Mdown){
		mouseoffset();
		ImouseX = mouseX;
		ImouseY = mouseY;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	GLfloat scaler=yoffset/10.f;
	zoom+=scaler;
	if(zoom <= 0){zoom= 0.1f;}
	
	offset[0]-=(mouseX*scaler);
	offset[1]+=(mouseY*scaler);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_Q){
		zoom+=0.1f;
	}
	if (key == GLFW_KEY_E){
		zoom-=0.1f;
		if(zoom <= 0){ zoom = 0.1f;}
	}
	if (key == GLFW_KEY_W){
		offset[1] = offset[1]-(0.1f/(zoom));
	}
	if (key == GLFW_KEY_S){
		offset[1] = offset[1]+(0.1f/(zoom));
	}
	if (key == GLFW_KEY_A){
		offset[0] = offset[0]+(0.1f/(zoom));
	}
	if (key == GLFW_KEY_D){
		offset[0] = offset[0]-(0.1f/(zoom));
	}
	if (key == GLFW_KEY_Z){
		recalc_trans(0.1f/PI);
	}
	if (key == GLFW_KEY_X){
		recalc_trans(-0.1f/PI);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS){
		fragData.conv_mode++;
		//if(fragData.conv_mode > 7) fragData.conv_mode=0;
		printf("Frag mode: %d\n",fragData.conv_mode);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS){
		fragData.conv_mode--;
		if(fragData.conv_mode < 0) fragData.conv_mode=0;
		printf("Frag mode: %d\n",fragData.conv_mode);
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS){
		fragData.bw_mode++;
		if(fragData.bw_mode > 4) fragData.bw_mode=0;
		printf("B/W mode: %d\n",fragData.bw_mode);
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS){
		fragData.bw_mode--;
		if(fragData.bw_mode < 0) fragData.bw_mode=4;
		printf("B/W mode: %d\n",fragData.bw_mode);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS){
		Picnum++;
		if(Picnum > (sizeof(Pictures)/sizeof(char*)-1)){Picnum=0;}
		printf("Switching to: %d: %s\n",Picnum, Pictures[Picnum]);
		reload_pic=true;
	}	
	if (key == GLFW_KEY_1 && action == GLFW_PRESS){
		Picnum--;
		if(Picnum < 0) {Picnum=sizeof(Pictures)/sizeof(char *)-1;}
		printf("Switching to: %d: %s\n",Picnum, Pictures[Picnum]);
		reload_pic=true;
	}	
		
}

void print_help(){
	printf("#############################################KEYS################################################\n");
	printf("Esc: Exit\n");
	printf("Q: Zoom in\n");
	printf("E: Zoom out\n");
	printf("W: Move down\n");
	printf("S: Move up\n");
	printf("A: Move right\n");
	printf("D: Move left\n");
	printf("Z: Rotate right\n");
	printf("X: Rotate left\n");
	printf("R: Increment convolution mode\n");
	printf("F: Decrement convolution mode\n");
	printf("T: Increment colour mode\n");
	printf("G: Decrement colour mode\n");
	printf("1: Prior Image\n");
	printf("2: Next Image\n");
	printf("############################### Frag modes ########################\n");
	printf("0: None\n");
	printf("1: Horizontal Sobel Edge\n");
	printf("2: Vertical Sobel Edge\n");
	printf("3: Merged Sobel Edge\n");
	printf("4: Unsharp\n");
	printf("5+: Gaussian Blur\n");
	printf("############################## B/W modes #################\n");
	printf("0 : None\n");
	printf("1 : L1\n");
	printf("2 : L2\n");
	printf("3 : L3\n");
	printf("4 : Sepia\n");
}


// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	print_help();
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_X, WINDOW_Y, "CPSC 453 Assignment 2", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetMouseButtonCallback(window,  mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	MyShader shader;
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// call function to create and fill buffers with geometry data
	MyGeometry geometry;
	if (!InitializeGeometry(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	MyTexture texture;
	if(!InitializeTexture(&geometry, &texture, Pictures[0], GL_TEXTURE_RECTANGLE))
		cout << "Program failed to intialize texture!" << endl;


	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		if(reload_pic){
			reload_pic=false;
			InitializeTexture(&geometry, &texture, Pictures[Picnum], GL_TEXTURE_RECTANGLE);
		}
		
		// call function to draw our scene
		RenderScene(&geometry, &texture, &shader); //render scene with texture
		CheckGLErrors();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	DestroyShaders(&shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
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
