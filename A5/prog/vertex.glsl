// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 UV;

out vec3 FragNormal;
out vec2 FragUV;


#define PI 3.1415926535897

#define torad(X) (X*PI/180)

#define IDENTITY4 mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)

uniform mat4 cameraMatrix=IDENTITY4;
uniform mat4 perspectiveMatrix=IDENTITY4;
uniform mat4 modelviewMatrix = IDENTITY4;

// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	FragNormal = normalize(
					(modelviewMatrix*vec4(VertexNormal, 0.f)).xyz
				);

	FragUV = UV;

	gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*(vec4(VertexPosition, 1.0));
}
