// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage

in vec2 uv;


// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D image;



void main(void)
{
    //float res = 0.001f;
    //vec2 newCoords;
    //newCoords.x = res * (int(textureCoords.x)/res);
    //newCoords.y = res * (int(textureCoords.y)/res);

    // write colour output without modification
//    vec4 colour = texture(tex, newCoords);


	vec4 colour = texture(image,uv);
    FragmentColour = colour;
}
