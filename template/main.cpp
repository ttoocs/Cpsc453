//Scott Saunders
//10163541
//Cpsc 453 template 
//October 1st, 2016.

// #include "main.h"  // Contains all the includes for this.
//	#include "gl_helpers.h"	//AUGH
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>

#include "gl_helpers.cpp"

using namespace std;
int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(512,512,"Scott Saunders - Template");	//Init window.

	while(!glfwWindowShouldClose(window)){ //Main loop.
    	glfwSwapBuffers(window);
	    glfwPollEvents();
	}

	glfwTerminate();	//Kill the glfw interface


}
