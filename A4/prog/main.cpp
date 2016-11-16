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
	GLuint cprog;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint compShader;
	GLuint tex_output;
	
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

		glstuff.cprog = glCreateProgram();
		glstuff.compShader = CompileShader(GL_COMPUTE_SHADER,LoadSource("compute.glsl"));
			
		glAttachShader(glstuff.prog, glstuff.vertexShader);
		glAttachShader(glstuff.prog, glstuff.fragShader);
		glAttachShader(glstuff.cprog, glstuff.compShader);

		//Attrib things here

//		cout << "Nuggets" << endl;	//I have no idea why I need to print something out here.
		glLinkProgram(glstuff.prog);
		check_gllink(glstuff.prog);
		glLinkProgram(glstuff.cprog);
		check_gllink(glstuff.cprog);
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
		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER, glstuff.vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindVertexArray(0);

		//Texture stuff
		
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
		glBindImageTexture(0, glstuff.tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F); //unit,texture,level,layrered,layer,access,format
			//Binds it to layout 0, .tex_output. level 0, <false>, 0 , access, format

		GLuint tmp = glGetUniformLocation(glstuff.prog,"dimentions");
		glUniform2i(tmp,WIDTH,HEIGHT);
		
		
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

char *pixels;

void to_ppm(){
	if(pixels != NULL)
		free(pixels);
	pixels = (char *) malloc(WIDTH*HEIGHT*sizeof(char)*4);
	
	glActiveTexture(GL_TEXTURE0);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
//	glGetTextureImage(glstuff.tex_output,0,GL_RGBA,GL_UNSIGNED_BYTE,WIDTH*HEIGHT*4,&pixels);
	FILE * out =fopen("out.ppm","wt");
	fprintf(out,"P3\n");
	fprintf(out,"%d %d\n %d \n",WIDTH,HEIGHT,127);
	int k=0;
	for(int i =0; i < HEIGHT ; i++){
		for(int j = 0; j < WIDTH ; j++){
			fprintf(out," %d %d %d ", pixels[k], pixels[k+1], pixels[k+2]);
			k+=4; //4 due to alpha.
		}
		fprintf(out,"\n");
	}
	fclose(out);
//	exit(1);
}

void Render(){
	glUseProgram(glstuff.cprog);
	glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(glstuff.prog);
	glBindVertexArray(glstuff.vertexarray);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glstuff.tex_output);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	
	to_ppm();	 

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
