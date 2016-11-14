// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;

uniform vec2 mouse_coord;

uniform int width;
uniform int height;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

void main(void)
{
  vec2 pixel_coord = vec2(gl_FragCoord.x/width,gl_FragCoord.y/height);

  float dist=(1-clamp(distance(mouse_coord,pixel_coord),0,1));

  dist= dist*dist*dist;

  FragmentColour = //vec4(dist, dist, dist, 1);
  FragmentColour = vec4(1, 0, 0, 1);
}
