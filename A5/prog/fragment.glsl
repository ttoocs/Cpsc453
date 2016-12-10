// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 430

// interpolated colour received from vertex stage

in vec2 FragUV;
in vec3 FragNormal;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform ivec2 dimentions;
uniform sampler2D image;

void main(void)
{
	vec4 colour;
	vec2 coord = FragUV;


//	coord.x /= dimentions.x;
//	coord.y /= dimentions.y;

	coord.x = coord.x + 1f;
	coord.y = coord.y + 1f;

	coord.x /= 2;
	coord.y /= 2;

//	colour = texture(image,uv);
	colour = texture(image,FragUV);
	
//	colour = vec4(FragNormal,0);

//	colour = vec4(vec3(gl_FragCoord.z),1.0f);

    FragmentColour = colour;
}
