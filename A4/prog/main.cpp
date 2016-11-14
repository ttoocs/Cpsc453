//Scott Saunders
//10163541
//Cpsc 453 template 
//October 1st, 2016.

#include <GL/glew.h>
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

#include "gl_helpers.cpp"
#include "parser.cpp"

#define WIDTH 512
#define HEIGHT 512



using namespace std;

int scene=0;
bool initalized=false;

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
		//Initalize stuff
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
	return;
}

int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Assignment 4");	//Init window.

	changeScene();	//Load up a scene!
	

		
	while(!glfwWindowShouldClose(window)){ //Main loop.
		Render();
	    	glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();	//Kill the glfw interface

}
