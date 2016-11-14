#version 410
layout(vertices = 2) out;

void main()
{
  gl_TessLevelOuter[0] = 20;
  gl_TessLevelOuter[1] = 1;

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
