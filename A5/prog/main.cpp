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
#include "shapes.cpp"
#include "planets.h"

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

float speed = 1;
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


}

// */
// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float move = PI/200.f;

		if(key == GLFW_KEY_E)
			cam.rotateCamera(0,-move);
		if(key == GLFW_KEY_Q)
			cam.rotateCamera(0,move);
			
		if(key == GLFW_KEY_W){
			cam.pos += cam.dir*move;
		}
		if(key == GLFW_KEY_S){
			cam.pos -= cam.dir*move;
		}
		if(key == GLFW_KEY_A){
			cam.rotateCamera(-move,0);
		}
		if(key == GLFW_KEY_D){
			cam.rotateCamera(move,0);
		}
		if(key == GLFW_KEY_Z){
			speed -= 0.001f;
		}
		if(key == GLFW_KEY_X){
			speed += 0.001f;
		}
		if(key == GLFW_KEY_P){
			speed = 0;
		}

		if(length(cam.pos) < 0.2){
			cam.pos = cam.pos*1.1f;
		}
		if(length(cam.pos) > 10){
			cam.pos = cam.pos/1.01f;
		}
		

}
bool mousePressed;
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) )
    mousePressed = !mousePressed;
}


vec2 mousePos;
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);

  vec2 diff = newPos - mousePos;
  if(mousePressed)
    cam.rotateCamera(-diff.x, diff.y);

  mousePos = newPos;
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
	


	
void Render(Object objs[4]){
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	for(int i =0; i < 4; i++){
		Object *s = &objs[i];

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



		//This bit is currently broken. Incorrectly adapting the modelviews.
			//Update model-view uniform
			mat4 modelview = mat4(s->modelview); //Supposed to set it to the predicesors.
			for(int j=i; j>0; j--){
				modelview[3] += objs[j].modelview[3];
			}
	
		glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "modelviewMatrix"),
	            1,
	            false,
	            &modelview[0][0]);

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

	return;
}
int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Assignment 5");	//Init window.

	glDebugMessageCallback(	GL_error_callback, NULL);
	glEnable(GL_DEBUG_OUTPUT);								//DEBUG :D
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS);	

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);	


	initalize_GL();

	Update_Perspective();	//updates perspective uniform, as it's never changed.

	#define MOON bodies[3]
	#define EARTH bodies[2]
	#define SUN bodies[1]
	#define SPACE bodies[0]
	
	#define SPHERE_QUAL 128	

	cam.pos = glm::vec3(0,0,4);
	Object bodies[4];
	
	#define mylog(X) log(X)/50.f //Note, this is from KM, to 0->10ish space.

	#define tilttovec(X)	vec3(0,cos(X),0)

	float sun_r		=	mylog(SUN_RADIUS);
	float earth_r = mylog(EARTH_RADIUS);
	float moon_r  = mylog(MOON_RADIUS);
	
	generateSphere(&SPACE,10,SPHERE_QUAL,SPHERE_QUAL);
	loadTexture(&SPACE,"./textures/notquite_space.png");
	rotateObjPos(&SPACE,glm::vec3(-1,0,0),torad(90));	//Rotate object for texture

	generateSphere(&SUN,sun_r,SPHERE_QUAL,SPHERE_QUAL);
	loadTexture(&SUN,"./textures/texture_sun.jpg");
	rotateObjPos(&SUN,glm::vec3(-1,0,0),torad(90));		//Rotate object for texture

	generateSphere(&EARTH,earth_r,SPHERE_QUAL,SPHERE_QUAL);
	loadTexture(&EARTH,"./textures/texture_earth_surface.jpg");
	rotateObjPos(&EARTH,glm::vec3(-1,0,0),torad(90)); //Rotate object for texture

	generateSphere(&MOON,moon_r,SPHERE_QUAL,SPHERE_QUAL);
	loadTexture(&MOON,"./textures/texture_moon.jpg");
	rotateObjPos(&MOON,glm::vec3(-1,0,0),torad(90));	//Rotate object for texture

	moveObj(&EARTH,vec3(0,0,mylog(EARTH_P_DIST)*5));
	moveObj(&MOON,vec3(0,0,mylog(MOON_P_DIST)*5));

	speed =0.01;
	while(!glfwWindowShouldClose(window))
	{ //Main loop.
		Render(bodies);
    glfwSwapBuffers(window);
		glfwPollEvents();

		
		rotateObjPos(&SUN,tilttovec(torad(SUN_DEG)),SUN_ROTATION*speed);
		rotateObjPos(&EARTH,tilttovec(torad(EARTH_DEG)),EARTH_ROTATION*speed);
		rotateObjPos(&MOON,tilttovec(torad(MOON_DEG)),MOON_ROTATION*speed);

		rotateObj(&EARTH,vec3(0,1,0),EARTH_ORBIT*speed);
		rotateObj(&MOON,vec3(0,1,0),MOON_ORBIT*speed);
	}
	glfwTerminate();	//Kill the glfw interface
}
