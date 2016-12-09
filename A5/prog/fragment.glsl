// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 430

// interpolated colour received from vertex stage

in vec2 uv;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform ivec2 dimentions;
uniform sampler2D image;

void main(void)
{
	vec4 colour;
	vec2 coord = uv;


//	coord.x /= dimentions.x;
//	coord.y /= dimentions.y;

	coord.x = coord.x + 1f;
	coord.y = coord.y + 1f;

	coord.x /= 2;
	coord.y /= 2;

//	colour = texture(image,uv);
	colour = texture(image,coord);
	
	colour = vec4(0.3);
    FragmentColour = colour;
}
