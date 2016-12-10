//Scott Saunders
//10163541
//Cpsc 453 template 
//October 1st, 2016.

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926535897939

#include	"glm/glm.hpp"
#include	"glm/gtc/matrix_transform.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "gl_helpers.cpp"
#include "camera.cpp"
#include "genShapes.cpp"


#define WIDTH 512*2
#define HEIGHT 512*2

//#define WIREFRAME
#define DEBUG

#ifdef DEBUG
	#define DEBUGMSG	printf("\n\n\t\t DEBUG MESSAGE AT LINE:\t%d\t In file:\t%s\n\n",__LINE__,__FILE__);
#else 
	#define DEBUGMSG	;
#endif


#define torad(X)	((float)(X*PI/180.f))

// #define V_PUSH(X,a,b,c) X.push_back(a); X.push_back(b); X.push_back(c);
#define V_PUSH(X,a,b,c) X.push_back(vec3(a,b,c));	//Re-wrttien for GLM.


using namespace std;

Camera cam;

/*
GLfloat step = 0.1;
GLfloat trans[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
GLfloat offset[3] = {0,0,0};
GLfloat theta = 0.f;
GLfloat FOV = PI/6.f;

void recalc_trans(GLfloat newtheta){
	theta += newtheta;
	trans[0][0]=cos(theta);
	trans[0][2]=-sin(theta);
	trans[2][0]=sin(theta);
	trans[2][2]=cos(theta);

//	GLfloat newoff[3] = {
//		offset[0]*cos(newtheta)+offset[2]*sin(newtheta),
//		offset[1],
//		-offset[0]*sin(newtheta)+offset[2]*cos(newtheta)};
//		offset[2]};
//	offset[0]=newoff[0];
//	offset[1]=newoff[1];	//Could be done better, but alas.
//	offset[2]=newoff[2];
}

void add_offset(float addoffset[3]){
	GLfloat newoff[3] = {
		addoffset[0]*cos(theta)+addoffset[2]*sin(theta),
		addoffset[1],
		-addoffset[0]*sin(theta)+addoffset[2]*cos(theta)};
	offset[0]+=newoff[0];
	offset[1]+=newoff[1];	//Could be done better, but alas.
	offset[2]+=newoff[2];
	
}

void reset_trans(){
	recalc_trans(-theta);
	offset[0]=0;
	offset[1]=0;
	offset[2]=6;
	FOV = PI/6;
}

*/


struct GLSTUFF{
	GLuint prog;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint vertexarray;
	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint uvsbuffer;
	GLuint indiciesbuffer;
	GLuint texture;
};
GLSTUFF glstuff;



/*
void set_uniforms(){
	glUseProgram(glstuff.cprog);
//	cout << "updating uniforms" << endl;
	GLuint uniFOV,uniOff,uniTrans;

	uniFOV =   glGetUniformLocation(glstuff.cprog,"FOV");
	uniOff =   glGetUniformLocation(glstuff.cprog,"offset");
	uniTrans = glGetUniformLocation(glstuff.cprog,"transform");
	
	glUniform1f(uniFOV,FOV);
	glUniform3fv(uniOff,1,offset);
	glUniformMatrix3fv(uniTrans,1,0,*trans);
}
*/
void initalize_GL(){
	
		glEnable(GL_DEPTH_TEST); 		//Turn on depth testing
		glDepthFunc(GL_LEQUAL); 			//Configure depth testing
		//glDepthFunc(GL_ALWAYS);

		#ifdef WIREFRAME
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		#endif
	
		//OpenGL programs
		glstuff.prog = glCreateProgram();
		glstuff.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource("vertex.glsl"));
		glstuff.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSource("fragment.glsl"));
			
		glAttachShader(glstuff.prog, glstuff.vertexShader);
		glAttachShader(glstuff.prog, glstuff.fragShader);

			//Attrib things here

		glLinkProgram(glstuff.prog);	//Link to full program.
		check_gllink(glstuff.prog);

		//Vertex stuffs

		glUseProgram(glstuff.prog);
		glGenVertexArrays(1, &glstuff.vertexarray);
		glGenBuffers(1, &glstuff.vertexbuffer);
		glGenBuffers(1, &glstuff.normalbuffer);
		glGenBuffers(1, &glstuff.uvsbuffer);
		glGenBuffers(1, &glstuff.indiciesbuffer);
	
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
		glEnableVertexAttribArray(0);

		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.normalbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0); //Normals
		glEnableVertexAttribArray(1);

		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.uvsbuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0); //UVS
		glEnableVertexAttribArray(2);
		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuff.indiciesbuffer);	//Indices

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Texture stuff

		glGenTextures(1,&glstuff.texture);
		glBindTexture(GL_TEXTURE_2D, glstuff.texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Sets paramiters of the texture.
		
		
	/*
		float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		glGenTextures(1, &glstuff.tex_output);	//tex.output is refrance to an array
		glActiveTexture(GL_TEXTURE0);		// ??? Makes the 0th texture active...
		glBindTexture(GL_TEXTURE_2D, glstuff.tex_output);	// bind a named texture to a texturing target  //Connect tex_output to GL_TEXTURE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );//GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );// GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Sets paramiters of the texture.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL); //target,level,internal,width,height,0,format,type,data
		glBindImageTexture(0, glstuff.tex_output, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); //unit,texture,level,layrered,layer,access,format
			//Binds it to layout 0, .tex_output. level 0, <false>, 0 , access, format

		GLuint tmp = glGetUniformLocation(glstuff.prog,"dimentions");
		glUniform2i(tmp,WIDTH,HEIGHT);


		//Buffer stuff
		glGenBuffers(1,&glstuff.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, glstuff.ssbo);
		*/
	


}

// */
// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float move = PI/200.f;

    if(key == GLFW_KEY_W)
      cam.pos += cam.dir*move;
    if(key == GLFW_KEY_S)
      cam.pos -= cam.dir*move;
    if(key == GLFW_KEY_D)
      cam.pos += cam.right*move;
    if(key == GLFW_KEY_A)
      cam.pos -= cam.right*move;
    if(key == GLFW_KEY_E)
      cam.pos += cam.up*move;
    if(key == GLFW_KEY_Q)
      cam.pos -= cam.up*move;
		if(key == GLFW_KEY_Z)
			cam.rotateCamera(move,0);
		if(key == GLFW_KEY_X)
			cam.rotateCamera(-move,0);

}

void Update_Perspective(){
	glm::mat4 perspectiveMatrix = glm::perspective(torad(80.f), 1.f, 0.1f, 20.f);
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);
}

void Update_Uniforms(){
	glm::mat4 camMatrix = cam.getMatrix();
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

}
	


void Render_Object(Object *s){	//Renders an individual object.
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(glstuff.prog);
	glBindVertexArray(glstuff.vertexarray);
	glUseProgram(glstuff.prog);
	
	Update_Uniforms();

	//Copy data:
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);	//Setup data-copy (points)
		glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*s->positions.size(),s->positions.data(),GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER,glstuff.normalbuffer);	//Setup data-copy (norms)
		glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*s->normals.size(),s->normals.data(),GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER,glstuff.uvsbuffer);	//Setup data-copy (uvs)
		glBufferData(GL_ARRAY_BUFFER,sizeof(vec2)*s->uvs.size(),s->uvs.data(),GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER,glstuff.indiciesbuffer);	//Setup data-copy	(indicies)
		glBufferData(GL_ARRAY_BUFFER,sizeof(unsigned int)*s->indices.size(),s->indices.data(),GL_DYNAMIC_DRAW);

	//Setup texture: (IE, load them)
	if(((*s).texture.data) != NULL){
		if(s->texture.components==3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->texture.tWidth, s->texture.tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (const void *) s->texture.data);
    else if(s->texture.components==4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->texture.tWidth, s->texture.tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void *) s->texture.data);

	} 
	//Actually draw.

	glDrawElements(
		GL_TRIANGLES,   //What shape we're drawing  - GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
		s->indices.size(),    //How many indices
		GL_UNSIGNED_INT,  //Type
		0
	);

}
	
void Render(){
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(glstuff.prog);


	glBindVertexArray(glstuff.vertexarray);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, glstuff.tex_output);
//	glDrawElements(GL_TRIANGLE,
	return;
}
int main(int argc, char * argv[]){



	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Assignment 5");	//Init window.

	glDebugMessageCallback(	GL_error_callback, NULL);
	glEnable(GL_DEBUG_OUTPUT);								//DEBUG :D
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS);	

	glfwSetKeyCallback(window, KeyCallback);

	initalize_GL();

	Update_Perspective();	//updates perspective uniform, as it's never changed.

	Object testsphere;

	generateSphere(&testsphere,1,1024,1024);
	//generateTorus(&testsphere,0.5f,0.25f,100,20);
//	generateTri(&testsphere);

	loadTexture(&testsphere,"./textures/texture_earth_surface.jpg");

	printf("sizeof %d \n\n",testsphere.positions.size());
	while(!glfwWindowShouldClose(window))
	{ //Main loop.
		Render_Object(&testsphere);
    glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();	//Kill the glfw interface
}
