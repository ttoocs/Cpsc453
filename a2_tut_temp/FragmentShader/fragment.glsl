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

//uniform sampler2D image;		//Syntax for accessing texture
uniform vec2 mousePos;

//www.opengl.org/wiki/Data_Type_(GLSL) for more information
void swizzleExample()
{
	vec4 a = vec4(1.0, 2.0, 3.0, 4.0);	//(x, y, z, w), (r, g, b, a), (s, t, p, q)
	vec3 b = a.xzx;						//b = (1.0, 3.0, 1.0)
	
	vec4 c = vec4(a.zw, b.xy);				//c = (3.0, 4.0, 1.0, 3.0)

	vec2 d = c.st;						//d = (1.0, 2.0)
	vec3 e = a.rgb;						//e = (1.0, 2.0, 3.0)

	mat4 m1;			//4d matrix declaration
	m1[0][2] = 5.0;		//Second row of the first column of m1 is 5.0
	m1[0] = a;			//First column of m1 is now (1.0, 2.0, 3.0, 4.0)
	b = m1[3].xyz;		//b = 3rd column of m1
}

void main(void)
{
//	vec4 color = texture(image, uv);

	vec3 color = vec3(uv, 0);
	FragmentColour = vec4(color, 1);
}
