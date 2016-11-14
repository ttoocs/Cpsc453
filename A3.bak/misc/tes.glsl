#version 410
layout(isolines, equal_spacing) in;

void main()
{
  float lines=gl_TessLevelOuter[0];
  float line_index=lines*gl_TessCoord.y;
  float segments=2*lines-1;
  // float u = gl_TessCoord.x / segments +
  //           2 * (1/segments) * gl_TessCoord.y;
  float u = (gl_TessCoord.x + 2 * line_index) /segments;
gl_Position = (1-u) * gl_in[0].gl_Position +
              u * gl_in[1].gl_Position;
}
