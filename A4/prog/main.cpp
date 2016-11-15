//Scott Saunders
//10163541
//Cpsc 453 template 
//October 1st, 2016.

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include <stdlib.h> 

#include "gl_helpers.cpp"
#include "parser.cpp"

#define WIDTH 512
#define HEIGHT 512

using namespace std;
int scene=0;
bool initalized=false;

GLfloat vertices[]={
	-1,	1,
	-1,	-1,
	1,	1,
	1,	-1,
};

struct GLSTUFF{
	GLuint vertexbuffer;
	GLuint vertexarray;
	GLuint prog;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint compShader;
	
};
GLSTUFF glstuff;



#define MAX_SCENE 2
void changeScene(){
	
	if(scene > MAX_SCENE)
		scene = 0;
	if(scene < MAX_SCENE )
		scene = MAX_SCENE;
	if(scene == 0){
		vector<GLfloat> objects = parse("scene1.txt");
	}else if(scene == 1){
		vector<GLfloat> objects = parse("scene2.txt");
	}else if(scene == 2){
		vector<GLfloat> objects = parse("scene3.txt");
	}

	if(!initalized){
		initalized=true;
		glstuff.prog = glCreateProgram();
		glstuff.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource("vertex.glsl"));
		glstuff.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSource("fragment.glsl"));
		//glstuff.compShader = CompileShader(GL_COMPUTE_SHADER,LoadSource("compute.glsl"));
			
		glAttachShader(glstuff.prog, glstuff.vertexShader);
		glAttachShader(glstuff.prog, glstuff.fragShader);
		//glAttachShader(glstuff.prog, glstuff.compShader);

		//Attrib things here

//		cout << "Nuggets" << endl;	//I have no idea why I need to print something out here.
		LinkProgram(glstuff.prog);
		
		//Vertex stuffs

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
		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER, glstuff.vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Texture stuff
		
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindVertexArray(0);
	}

		//Update stuff
	
}


// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
		scene++;
    	changeScene();
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
		scene--;
		changeScene();
    }

}


void Render(){
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(glstuff.prog);
	glBindVertexArray(glstuff.vertexarray);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	return;
}
int main(int argc, char * argv[]){


	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Assignment 4");	//Init window.

	glDebugMessageCallback(	GL_error_callback, NULL);
	glEnable(GL_DEBUG_OUTPUT);								//DEBUG :D
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS);	

	changeScene();	//Load up a scene!

		Render();

	while(!glfwWindowShouldClose(window)){ //Main loop.
    	glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();	//Kill the glfw interface
}
