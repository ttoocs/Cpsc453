#version 430

/*
Compute shader variables:
uvec3 	gl_numWorkGroups
uvec3 	gl_WorkGroupSize
uvec3 	gl_WorkGroupID
uvec3 	gl_LocalInvcationID
uvec3 	gl_GlobalInvocationID
uint 	gl_LocalInvocationID


*/



layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

int ray_intersect_tri(uint obj){
	return 0;
}
int ray_intersect_sphere(uint obj){
	return 0;
}
int ray_intersect_plane(uint obj){
	return 0;
}
int ray_intersect_point(uint obj){
	return 0;
}

// unifrom float objets[];

void main(){
  	ivec2 dims = imageSize (img_output);

	vec4 colour = vec4(0);
  	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

  	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

//	vec2 coords = vec2(gl_GlobalInvocationID.xy*2 - 512) /512;
	vec2 coords = vec2(gl_GlobalInvocationID.xy);
	
	//colour += vec4(coords,0,0);

	if(coords.x == 0 ) { colour = vec4(0,0,1,0);}
	if(coords.y == 0 ) { colour = vec4(0,0,1,1);}
//	if(coords.x == 256 ) { colour = vec4(1,0,1,1);}
//	if(coords.y > 0 ) { colour += vec4(0,1,0,1);}
	

	colour = vec4(0.2);
//	ivec2	offset=ivec2(-1,-1);
  	imageStore(img_output, pixel_coords, colour);
}

