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

#include "GlyphExtractor.h"


// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define vpush(X, Y) vertices.push_back(X); vertices.push_back(Y);
#define cpush(X, Y, Z) colours.push_back(X) ; colours.push_back(Y) ; colours.push_back(Z)

//NOTE: the last vector encodes the level (redudently 3 times, but that's due to vec size)
//These encode the level with the type being pushed.
//
//


#define v1push(X1,Y1,X2,Y2) vpush(X1,Y1); vpush(X2,Y2); vpush(0,0) ; vpush(0,0); vpush(1,1); 
#define v2push(X1,Y1,X2,Y2,X3,Y3) vpush(X1,Y1); vpush(X2,Y2); vpush(X3,Y3) ; vpush(0,0); vpush(2,2);
#define v3push(X1,Y1,X2,Y2,X3,Y3,X4,Y4) vpush(X1,Y1); vpush(X2,Y2); vpush(X3,Y3) ; vpush(X4,Y4); vpush(3,3);


#define lincol c1push(0,0.6f,0,0,0.6f,0);

//Wrapper functions for the above, that allow for control points and control polygons. 
#define v1pushc(X1,Y1,X2,Y2) v1push(X1,Y1,X2,Y2); if(cntrl_pnts){ pushcpnt(X1,Y1) ; pushcpnt(X2,Y2);} 
#define v2pushc(X1,Y1,X2,Y2,X3,Y3) v2push(X1,Y1,X2,Y2,X3,Y3); if(cntrl_pnts){ pushcpnt(X1,Y1) ; pushncpnt(X2,Y2) ; pushcpnt(X3,Y3); v1push(X1,Y1,X2,Y2);v1push(X2,Y2,X3,Y3); lincol; lincol; }
#define v3pushc(X1,Y1,X2,Y2,X3,Y3,X4,Y4) ; v3push(X1,Y1,X2,Y2,X3,Y3,X4,Y4) if(cntrl_pnts){ pushcpnt(X1,Y1) ; pushncpnt(X2,Y2) ; pushncpnt(X3,Y3); pushcpnt(X4,Y4); v1push(X1,Y1,X2,Y2);v1push(X2,Y2,X3,Y3); v1push(X3,Y3,X4,Y4); lincol; lincol; lincol;}


//Likeswise for colours
#define c1push(X1,Y1,Z1,X2,Y2,Z2) cpush(X1,Y1,Z1); cpush(X2,Y2,Z2); cpush(0,0,0) ; cpush(0,0,0); cpush(1,1,1)
#define c2push(X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3) cpush(X1,Y1,Z1); cpush(X2,Y2,Z3); cpush(X3,Y3,Z3) ; cpush(0,0,0); cpush(2,2,2)
#define c3push(X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3,X4,Y4,Z4) cpush(X1,Y1,Z1); cpush(X2,Y2,Z2); cpush(X3,Y3,Z3) ; cpush(X4,Y4,Z4); cpush(3,3,3)

#define pushscale  0.03f/zoom //0.3f/zoom

#define pushplus(X,Y,A,B,C) v1push(X+pushscale,Y , X-pushscale,Y); v1push(X,Y+pushscale,X,Y-pushscale); c1push(A,B,C,A,B,C); c1push(A,B,C,A,B,C)
#define pushcpnt(X,Y) pushplus(X,Y,0,0.6f,0.4f);
#define pushncpnt(X,Y) pushplus(X,Y,0.6f,0,0.4f);


using namespace std;

double mouse_x;
double mouse_y;
int width;
int height;
int scene=0;
float scrollspeed=0.00f;

bool cntrl_pnts=true;

GLint mouse_coord_uniform;
GLint width_uniform;
GLint height_uniform;

string font="";

vector<GLfloat> vertices;
vector<GLfloat> colours;

float c = 0;
GLfloat zoom = 0.5f;
GLfloat trans[2][2] = {{1,0},{0,1}};    //2D transformation Matrix
GLfloat offset[2] = {0,0};              //2D offset vector
GLfloat theta = 0;

// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint, GLuint, GLuint fragmentShader);
void RenderScene(bool clear);


// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering
//

struct MyShader
{
    // OpenGL names for vertex and fragment shaders, shader program
    GLuint  vertex;
    GLuint  fragment;
    GLuint  TCS, TES;
    GLuint  program;

    // initialize shader and program names to zero (OpenGL reserved value)
    MyShader() : vertex(0), fragment(0), program(0)
    {}
};
MyShader shader;

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
    // load shader source from files
    string vertexSource = LoadSource("vertex.glsl");
    string fragmentSource = LoadSource("fragment.glsl");
    string tcsSource = LoadSource("tcs.glsl");
    string tesSource = LoadSource("tes.glsl");
    if (vertexSource.empty() || fragmentSource.empty() || tcsSource.empty() || tesSource.empty()) return false;

    // compile shader source into shader objects
    shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    shader->TCS = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
    shader->TES = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);
    shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // link shader program
    shader->program = LinkProgram(shader->vertex, shader->TCS, shader->TES, shader->fragment);

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
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
    // OpenGL names for array buffer objects, vertex array object
    GLuint  vertexBuffer;
    GLuint  colourBuffer;
    GLuint  vertexArray;
    GLsizei elementCount;

    // initialize object names to zero (OpenGL reserved value)
    MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
    {}
};
MyGeometry geometry;

void update_verts(){
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	printf("Size is: %lu\n",vertices.size());
    geometry.elementCount = vertices.size()/2;
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);	
    glBindBuffer(GL_ARRAY_BUFFER, geometry.colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, colours.size()*sizeof(GLfloat), colours.data(), GL_STATIC_DRAW);
}
void update_level(int level){
	GLint uniLvl = glGetUniformLocation(shader.program, "level");
    glUniform1i(uniLvl, level);

}

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry)
{
    // three vertex positions and assocated colours of a triangle
    zoom = 1;

	pushcpnt(-0.4,.5);
	pushcpnt(0.4,.5);
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2push(-.5,-.5, 0,-1, .5,-.5);
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2push(-.5,-.5, 0,-0.8, .5,-.5);
	
	
/*
	v1push( -1, -1, 1, 1);
	v3push( -0.5, -0.5, -0.5, 0.5, 0.5, -0.5 ,  0.5, 0.5 );
	v3push( -0.8, -0.8, -0.8, 0.5, 0.8,  0.8 , -0.8, 0.8 );
	v3push( -0.5, 0.5,  -0.5, 0.8, 0.5, -0.5 ,  0.5, 0.8 );
	c1push( 1,1,1, 1,1,1);
	c3push( 1,1,1, 1,1,1,1,1,1,1,1,1);
	c3push( 1,1,1, 1,1,1,1,1,1,1,1,1);
	c3push( 1,1,1, 1,1,1,1,1,1,1,1,1);
*/

    // these vertex attribute indices correspond to those specified for the
    // input variables in the vertex shader
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;

    // create an array buffer object for storing our vertices
    glGenBuffers(1, &geometry->colourBuffer);
    // create another one for storing our colours
    glGenBuffers(1, &geometry->vertexBuffer);
	update_verts();


    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

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


void push_teapot(){
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2pushc(1, 1, 2, -1, 0, -1);
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2pushc(0, -1, -2, -1, -1, 1);
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2pushc(-1, 1, 0, 1, 1, 1);
	c2push(1,1,1, 1,1,1, 1,1,1);
	v2pushc(1.2, 0.5, 2.5, 1.0, 1.3, -0.4);
}
void push_fish(){
    c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
	v3pushc(1, 1, 4, 0, 6,  2, 9, 1);  //Fun fact: this expands to 90 different pushes!
    c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
	v3pushc(8, 2, 0, 8, 0, -2, 8, 4);
    c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
	v3pushc(5, 3, 3, 2, 3,  3, 5, 2);
    c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
	v3pushc(3, 2.2, 3.5, 2.7, 3.5, 3.3, 3, 3.8);
    c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
	v3pushc(2.8, 3.5, 2.4, 3.8, 2.4, 3.2, 2.8, 3.5);
}

GlyphExtractor extractor;
MyGlyph glyph;
MyContour contour;
MySegment segment;
string str_ng;
GLfloat adv;
void fontstuff(){
	extractor.LoadFontFile(font);
	adv=0;
	for(int i =0; i<str_ng.length(); i++){
		char c = str_ng[i];
		glyph = extractor.ExtractGlyph(c);
		for(int j =0; j<glyph.contours.size(); j++){
			contour = glyph.contours[j];
			for(int k=0; k<contour.size(); k++){
				segment = contour[k];
				switch(segment.degree){
					case 1:	{//LINEAR
							c1push(1,1,1, 1,1,1);
							v1pushc(segment.x[0]+adv,segment.y[0], segment.x[1]+adv,segment.y[1]);
							break;
					}
					case 2: {//QUADRATIC
							c2push(1,1,1, 1,1,1, 1,1,1);
							v2pushc(segment.x[0]+adv,segment.y[0], segment.x[1]+adv,segment.y[1], segment.x[2]+adv,segment.y[2]);
							break;
					}
					case 3: {//CUBIC
							c3push(1,1,1, 1,1,1, 1,1,1, 1,1,1);
							v3pushc(segment.x[0]+adv,segment.y[0], segment.x[1]+adv,segment.y[1], segment.x[2]+adv,segment.y[2], segment.x[3]+adv,segment.y[3]);
							break;
					}
					default: {
							printf("TOO MANY DEGREES FOR SEGMENT %d,%d,%d,%d,%c,%s \n",segment.degree,k,j,i,c,str_ng);
							break;
					}
				}
			}
			
		}
		adv += glyph.advance;
	}
}

//LOAD SCENE TODO
void loadscene(){
	#define MAXSCENE 6
//	printf("Scene pre: %d ",scene);
	if (scene > MAXSCENE)
		scene = 0;	
	else if (scene < 0)
		scene = MAXSCENE;
//	printf("Scene pos: %d \n",scene);	
	vertices.clear();
	colours.clear();
	scrollspeed=0;
	if(scene == 1){
		printf("Drawing fish\n");
		zoom=1.f/10.f;
		push_fish();
		offset[0] = 0;              //2D offset vector
		offset[1] = 0;
		adv=1;
	}else if (scene == 0){
		printf("Drawing teapot\n");
		zoom=0.5f;
		push_teapot();
		offset[0] = 0;              //2D offset vector
		offset[1] = 0;
		adv=1;
	}else if (scene == 2){
		printf("Font Lora \n");
		zoom=0.25f;
		font="fonts/Lora-Regular.ttf";
		str_ng="Scott Saunders.";
		fontstuff();	
		offset[0] = -3.25f;              //2D offset vector
		offset[1] = -0.25f;
	}else if (scene == 3){
		printf("Font Source Sans Pro\n");
		zoom=0.25f;
		font="fonts/SourceSansPro-Regular.otf";
		str_ng="Scott Saunders.";
		fontstuff();	
		offset[0] = -3.25f;              //2D offset vector
		offset[1] = -0.25f;
	}else if (scene == 4){
		printf("Font Leafy Glade\n");
		zoom=0.30f;
		font="fonts/Leafy-Glade.ttf";
		str_ng="Scott Saunders.";
		fontstuff();	
		offset[0] = -3.25f;              //2D offset vector
		offset[1] = -0.25f;
	}else if (scene == 5){
		printf("Font Alex Brush\n");
		zoom=0.5f;
		font="fonts/AlexBrush-Regular.ttf";
		str_ng="The quick brown fox jumps over the lazy dog.";
		fontstuff();	
		offset[0] = zoom*4;              //2D offset vector
		offset[1] = -0.25f;
		scrollspeed = 0.025f;
	}else if (scene == 6){
		printf("Inconsolata\n");
		zoom=0.5f;
		font="fonts/Inconsolata.otf";
		str_ng="The quick brown fox jumps over the lazy dog.";
		fontstuff();	
		offset[0] = zoom*4;              //2D offset vector
		offset[1] = -0.25f;
		scrollspeed = 0.025f;
	}
	update_verts();
	RenderScene(true);
	return;
}


void scroll(){
	offset[0] -= scrollspeed;
	if(offset[0] < -adv -2/zoom)
		offset[0] = 2/zoom;
	if(offset[0] > 2/zoom)
		offset[0] = -adv -2/zoom;
	//printf("Offset : %f\n",offset[0]);
}
// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(bool clear)
{
    // clear screen to a dark grey colour
    glClearColor(0.2, 0.2, 0.2, 1.0);
	if(clear)
    glClear(GL_COLOR_BUFFER_BIT);

    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(shader.program);

//    glUniform2f(mouse_coord_uniform, mouse_x, mouse_y);
//    glUniform1i(width_uniform, width);

    GLint uniTransform = glGetUniformLocation(shader.program, "transform");
    glUniformMatrix2fv(uniTransform, 1, GL_FALSE, *trans);

    GLint uniZoom = glGetUniformLocation(shader.program, "zoom");
    glUniform1f(uniZoom, zoom);

    GLint uniOffset = glGetUniformLocation(shader.program, "offset");
    glUniform2fv(uniOffset, 1, offset);

//    glUniform1i(height_uniform, height);

    glBindVertexArray(geometry.vertexArray);
    glDrawArrays(GL_PATCHES, 0, geometry.elementCount);

    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
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
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int oldscene = scene;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		scene --;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		scene ++;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		scene --;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		scene ++;
	if( scene != oldscene){
		loadscene();
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS){
		if(cntrl_pnts) {cntrl_pnts=false;} else {cntrl_pnts=true;}
		loadscene();
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		if(cntrl_pnts) {cntrl_pnts=false;} else {cntrl_pnts=true;}
		loadscene();
	}
	if (key == GLFW_KEY_Q) {
		scrollspeed += 0.01f;
	}
	if (key == GLFW_KEY_E) {
		scrollspeed -= 0.01f;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  glfwGetWindowSize(window,&width,&height);
  // glfwGetFramebufferSize(window,&width,&height);

  mouse_x=xpos/width;
  mouse_y=(1-(ypos/height));

//  std::cout << "Mouse Position (" << mouse_x << ", " << mouse_y << ")" << std::endl;
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
	glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(512, 512, "Scott Saunders, A3", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwMakeContextCurrent(window);

    // query and print out information about our OpenGL environment
    QueryGLVersion();

    // call function to load and compile shader programs
    //MyShader shader;
    if (!InitializeShaders(&shader)) {
        cout << "Program could not initialize shaders, TERMINATING" << endl;
        return -1;
    }
    glPatchParameteri(GL_PATCH_VERTICES,5);

    mouse_coord_uniform=glGetUniformLocation(shader.program,"mouse_coord");
    width_uniform=glGetUniformLocation(shader.program,"width");
    height_uniform=glGetUniformLocation(shader.program,"height");

    // call function to create and fill buffers with geometry data
    if (!InitializeGeometry(&geometry))
        cout << "Program failed to intialize geometry!" << endl;

    // run an event-triggered main loop
    while (!glfwWindowShouldClose(window))
    {
        // call function to draw our scene
        scroll();
        RenderScene(true);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

		
        // sleep until next event before drawing again
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
GLuint LinkProgram(GLuint vertexShader, GLuint TCS, GLuint TES, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (TCS) glAttachShader(programObject, TCS);
    if (TES) glAttachShader(programObject, TES);
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
