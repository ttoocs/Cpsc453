// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
// Modified by Scott Saunders
#version 430

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexColour;

// output to be interpolated between vertices and passed to the fragment stage
out vec2 uv;


void main()
{
    // assign vertex position without modification
    gl_Position = vec4(VertexPosition, 0.0, 1.0);

    // assign output colour to be interpolated
    uv = VertexPosition;
	//uv=vec2(0,0);
}
