#version 410
layout(isolines, equal_spacing) in;

void main()
{
    float total_lines = gl_TessLevelOuter[1];
    float line_index = total_lines * gl_TessCoord.y;
    float line_u = gl_TessCoord.x;
    float u = (2*line_index + line_u) / (2*total_lines-1) / 2;
    // gl_Position = u * gl_in[0].gl_Position + (1-u) * gl_in[1].gl_Position;
    gl_Position = vec4(gl_TessCoord.x,gl_TessCoord.y,0,1);
}
