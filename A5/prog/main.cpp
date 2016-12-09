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

#include "gl_helpers.cpp"
#include "camera.cpp"
				//Include Jeremy's camera.

#define WIDTH 512*1
#define HEIGHT 512*1


// #define V_PUSH(X,a,b,c) X.push_back(a); X.push_back(b); X.push_back(c);
#define V_PUSH(X,a,b,c) X.push_back(vec3(a,b,c));	//Re-wrttien for GLM.


using namespace std;


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

std::vector<glm::vec3> vertices;

struct GLSTUFF{
	GLuint vertexbuffer;
	GLuint vertexarray;
	GLuint prog;
	GLuint vertexShader;
	GLuint fragShader;
};
GLSTUFF glstuff;

typedef struct Sphere Sphere;
struct Sphere{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;			//What are these used for?
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	float radius;
	int divisions;
};

//Generates a sphere based on it's radious and divisions.
void generateCircle(Sphere s)
{
	float uStep = 1.f/(float)(s.divisions-1);
	#define vStep uStep

	float u = 0.f;

	//Traversing u
	for(int i=0; i<s.divisions; i++)
	{

		vec3 center = vec3(	s.radius*cos(2.f*PI*u), 0.f,	s.radius*sin(2.f*PI*u));

		float v = 0.f;

		//Traversing v
		for(int j=0; j<s.divisions; j++)
		{
//			vec3 pos = vec3(	(s.radius+t_r*cos(2.f*PI*v)) * cos(2.f*PI*u),
//								t_r*sin(2.f*PI*v),
//								(s.radius+t_r*cos(2.f*PI*v)) * sin(2.f*PI*u));

			vec3 pos = vec3(s.radius*cos(2.f*PI*v)*sin(2.f*PI*u),
											s.radius*sin(2.f*PI*v)*cos(2.f*PI*u),
											s.radius*cos(2.f*PI*u));
		
			vec3 normal = normalize(pos - center);	//No idea what this is or does.
			
			s.positions.push_back(pos);
			s.normals.push_back(normal);
			s.uvs.push_back(vec2(u, v));

			v += vStep;
		}

		u += uStep;
	}

	for(int i=0; i<s.divisions-1; i++)
	{
		for(int j=0; j<s.divisions -1; j++)
		{
			unsigned int p00 = i*s.divisions+j;
			unsigned int p01 = i*s.divisions+j+1;
			unsigned int p10 = (i+1)*s.divisions + j;
			unsigned int p11 = (i+1)*s.divisions + j + 1;

			s.indices.push_back(p00);
			s.indices.push_back(p10);
			s.indices.push_back(p01);
		}
	}
	#undef vStep
}

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
		glGenBuffers(1, &glstuff.vertexbuffer);
		glGenVertexArrays(1, &glstuff.vertexarray);
		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	
		//Push square

		V_PUSH(vertices,1,0,0);
		V_PUSH(vertices,0,1,0);
		V_PUSH(vertices,1,1,0);
		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER, glstuff.vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
 		glBindBuffer(GL_ARRAY_BUFFER, 0);
   	glBindVertexArray(0);

		//Texture stuff
		
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
/*    if (key == GLFW_KEY_1 && action == GLFW_PRESS){
	scene++;
    	changeScene();
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS){
	scene--;
	changeScene();
    }

    if(key == GLFW_KEY_Z)
	step -= 0.01;
    if(key == GLFW_KEY_X)
	step += 0.01;

    float addoffset[3]={0,0,0};
    if(key == GLFW_KEY_W){
	addoffset[2]-=step;	//FORWARD/BACK
//	cout << "Z: " << offset[2] << endl;
    }	
    if(key == GLFW_KEY_S){
	addoffset[2]+=step;
//	cout << "Z: " << offset[2] << endl;
    }
    if(key == GLFW_KEY_A){
 	recalc_trans(step/PI);
    }
    if(key == GLFW_KEY_D){
	recalc_trans(-step/PI);
    }
    if(key == GLFW_KEY_Q){
	addoffset[1]+=step;	//
    }
    if(key == GLFW_KEY_E){
	addoffset[1]-=step;
    }
    if(key == GLFW_KEY_R){
	FOV-=step/PI;		//FOV
//	cout << FOV << endl;
    }
    if(key == GLFW_KEY_F){
	FOV+=step/PI;
//	cout << FOV << endl;
    }
    if(key == GLFW_KEY_P){
	particles++;
	changeScene();
    }
    if(key == GLFW_KEY_L){
	particles--;
	changeScene();
    }
    add_offset(addoffset);
    set_uniforms();
	*/
}

void Render_Sphere(Sphere s){
	glUseProgram(glstuff.prog);
	glBindVertexArray(glstuff.vertexarray);
	
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
	while(!glfwWindowShouldClose(window))
	{ //Main loop.
		Render();
    glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();	//Kill the glfw interface
}
