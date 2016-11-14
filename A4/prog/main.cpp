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

#include "gl_helpers.cpp"
#include "parser.cpp"

#define WIDTH 512
#define HEIGHT 512

using namespace std;
int main(int argc, char * argv[]){

	parse("scene1.txt");
	/*
	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Assignment 4");	//Init window.

	while(!glfwWindowShouldClose(window)){ //Main loop.
    	glfwSwapBuffers(window);
	    glfwPollEvents();
	}
	

	glfwTerminate();	//Kill the glfw interface
	*/

}
