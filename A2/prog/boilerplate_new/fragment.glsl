// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 textureCoords;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2DRect tex;

struct fragment_data {
	int bw_mode=0;	// 0=none, 1= L1. ,2=L2, 3=L3, 4=sepia
	vec2 conv_size=[0,0]; // 0,0 = none, else the size of the convultion matrix
	int conv_bi=0; // 0 = none, 1 = Apply the convulution (R^nx1), twice, once veritcal, once horizontal.

	//TODO: A data-type for arbitrary sized matracies.
		
}
uniform fragment_data fragData;


void main(void)
{
    float res = 0.001f;
    vec2 newCoords;
    newCoords.x = res * (int(textureCoords.x)/res);
    newCoords.y = res * (int(textureCoords.y)/res);

    // write colour output without modification
    vec4 colour = texture(tex, newCoords);
	
/* //Black and white:
	float lum;
	lum = texture(tex, newCoords)[0]*0.213f; //RED
	lum += texture(tex, newCoords)[1]*0.715f; //GREEN
	lum += texture(tex, newCoords)[2]*0.072f; //BLUE
	vec4 colour(lum);
//	colour[0] = lum;
//	colour[1] = lum;
//	colour[2] = lum;
	colour[3] = texture(tex, newCoords)[3];


		//*/
    //colour = 1 - colour;

    FragmentColour = colour;
}
