#version 430

#define T_TRI 1
#define T_LIGHT 2
#define T_SPHERE 3
#define T_PLANE 4
#define T_POINT 5
#define T_PARTICLE 6

/*
Compute shader variables:
uvec3 	gl_numWorkGroups
uvec3 	gl_WorkGroupSize
uvec3 	gl_WorkGroupID
uvec3 	gl_LocalInvcationID
uvec3 	gl_GlobalInvocationID
uint 	gl_LocalInvocationID


*/




//ACCESS FUNCTIONS FOR OBJECTS.


#define OBJSIZE 13


#define OBJ objs
//Offset by 1, for the object[] size.
#define OBJ_BADDR(X) ((X*OBJSIZE)+1)
#define OBJ_ADDR(X,Y) OBJ_BADDR(X)+Y
#define OBJ_DATA(X,Y) OBJ[OBJ_ADDR(X,Y)]
#define OBJ_TOVEC3(X,Y) vec3(OBJ_DATA(X,Y),OBJ_DATA(X,Y+1),OBJ_DATA(X,Y+2))
#define OBJ_SETV3(X,Y,Z)  OBJ_DATA(X,Y)=Z.x; OBJ_DATA(X,Y)=Z.y; OBJ_DATA(X,Y)=Z.z;

#define num_objs int(OBJ[0])

#define obj_type(X) OBJ_DATA(X,0)
#define obj_colour(X) OBJ_TOVEC3(X,1)

#define set_s_c(X,Y) OBJ_SETV3(X,4,Y)
#define sphere_c(X) OBJ_TOVEC3(X,4)
#define sphere_r(X) OBJ_DATA(X,7)

#define tri_p1(X) OBJ_TOVEC3(X,4)
#define tri_p2(X) OBJ_TOVEC3(X,7)
#define tri_p3(X) OBJ_TOVEC3(X,10)

#define plane_n(X) OBJ_TOVEC3(X,4)
#define plane_p(X) OBJ_TOVEC3(X,7)
#define plane_ned(X) OBJ_DATA(X,8)

#define light_p(X) OBJ_TOVEC3(X,4)


#define PI 3.14159265358793
#define EPSILON 0.000001

struct ray{
	vec3 origon;
	vec3 direction;
};


//uniform vec4 cam = vec4(0,0,0,PI/3);

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

layout(std430, binding = 1) buffer object_buffer{
	float objs[];
};

int ERROR=0;
#define E_OTHER 1
#define E_TYPE 2
void set_error(int type){
	if(ERROR ==0)
		ERROR=type;
}
void error_out(ivec2 pixel_coords){	
	vec4 c = vec4(1,1,1,1);
/*	switch(ERROR){
		case (E_OTHER):
			c=vec4(0,0,1,0);
			break;

	}*/
	imageStore(img_output, pixel_coords, c);
}

float ray_intersect_sphere(ray r, uint obj_ID){
	
	float t1,t2;
	float a = dot(r.direction,r.direction);
	float b = 2* dot(r.direction, (r.origon - sphere_c(obj_ID)))  ;
	float c = dot((r.origon - sphere_c(obj_ID)),(r.origon - sphere_c(obj_ID)))-pow(sphere_r(obj_ID),2);


	float det = (pow(b,2) - 4*a*c);
	if(det < EPSILON){return -1;}
	t1 = (-b + sqrt(det))/2*a;
	t2 = (-b - sqrt(det))/2*a;

	if(t1 < t2 && t1 >= 0){
		return(t1);
	}else
		return(t2);
		
}

//#define TEST_CULL
float ray_intersect_triangle(ray r, uint obj){
	//Möller–Trumbore algorithm
	vec3 e1 = tri_p2(obj) - tri_p1(obj);	//SUB
	vec3 e2 = tri_p3(obj) - tri_p1(obj);	//SUB
	
	vec3 pvec = cross(r.direction, e2);		//CROSS
	float det = dot(e1,pvec);					//DOT

	#ifdef TEST_CULL
	if(det < EPSILON)
	#else
	if(det < EPSILON && det >-EPSILON)
	#endif
		return(-1);	
	vec3 tvec = r.origon - tri_p1(obj);
	float u = dot(tvec,pvec);
	if (u < 0.0 || u > det)
		return(-1);
	vec3 qvec = cross(tvec,e1);
	float v = dot(r.direction,qvec);
	if (v < 0.0 || u + v > det)
		return(-1);
	float t = dot(e2,qvec);
	t /= det;
	u /= det;
	v /= det;	
	return t;
}

float ray_intersect_plane(ray r, uint obj){
	//(dot(O +td -q, n) =0
	if(plane_ned(obj)==0 && vec2(gl_GlobalInvocationID.xy) == ivec2(0)){		//Probably causes some error on 1st frame.
		OBJ_SETV3(obj,3,normalize(plane_n(obj)));	
		plane_ned(obj) = 1;
	}

	float d = dot(r.direction,plane_n(obj));
	if(d < EPSILON && d > -EPSILON)
		return -1;
	float t = dot(plane_p(obj),plane_n(obj)) - dot(r.origon,plane_n(obj));

	t /= d;
	return t;

/*	float div = dot(r.direction,plane_n(obj));

	if(div < EPSILON)
		return -1;

	float t = (-dot(r.origon,plane_n(obj)) + dot(plane_n(obj),plane_p(obj)));
	t	/=	div;
	return t;
*/
}


float ray_intersect_point(ray r, uint obj){
	vec3 t= (-r.origon + light_p(obj));
	t.x /= r.direction.x;
	t.y /= r.direction.y;
	t.z /= r.direction.z;

	if(abs(t.x - t.y)< EPSILON && abs(t.x - t.z) < EPSILON && abs(t.y - t.z) < EPSILON){
		return t.x;
	} else
		return -1;
	

}

// unifrom float objets[];

float test_object_intersect(ray r, uint obj){
	switch(int(obj_type(obj))){
		case T_TRI:
			return ray_intersect_triangle(r,obj);	
//			break;
		case T_SPHERE:
			return ray_intersect_sphere(r,obj);
//			break;
		case T_PLANE:
			return ray_intersect_plane(r,obj);
//			break;
		case T_POINT:
			return ray_intersect_point(r,obj);
		case T_LIGHT:
			return(-1);
//			break;
		case T_PARTICLE:
			return ray_intersect_sphere(r,obj);
	}
	//If we get here, we have issues.
	set_error(E_TYPE);
}

vec4 test_objects_intersect(ray r){ //Tests _ALL_ objects
	float t;
	vec4 ret = vec4(0,0,0,-1);
	int i = 0;
	for(i=0; i < num_objs ; i++ ){
		t = test_object_intersect(r,i);
		if(ret.w < 0 || t < ret.w && t >= 0){
			ret.w = t;
			ret.xyz = obj_colour(i).xyz;
		}		
	}
	return(ret);
}


//TODO: SHADOWS
//TODO: DIFFUSE/SPECULAR REFLECTION
//TODO: PARTICLES
//TODO: MOVEMENT

void main(){
	
	vec4 cam = vec4(0,0,0,PI/3);

  	ivec2 dims = imageSize (img_output);

	vec4 colour = vec4(0);
  	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

  	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec2 coords = vec2(gl_GlobalInvocationID.xy)/256;
	coords = coords + vec2(-1);

	//Red-green-ness	
//	colour = vec4(abs(coords),0,0);
	
	ray cray;
	cray.origon = vec3(cam.xyz);

	cray.direction = vec3(coords, -1/tan(cam.w/2));
	cray.direction = normalize(cray.direction);

//	colour = (vec4(ray_intersect_sphere(cray,0)!=-1));
//	colour = (vec4(ray_intersect_triangle(cray,1)!=-1));
//	colour = (vec4(1/(ray_intersect_plane(cray,2))));

	vec4 res = test_objects_intersect(cray);
	if(res.w >= 0)
		colour = vec4(res.xyz,0);

//	colour = vec4(obj_type(3)/6);

//	colour = abs(vec4(plane_n(2),0));

		//Moving stuff, cause I can!
	if(pixel_coords.x == 0 && pixel_coords.y == 0){
//		set_s_c(0,sphere_c(0).x, sphere_c(0).y+0.01f,sphere_c(0).z);
//		OBJ_DATA(4,6) = OBJ_DATA(4,10) + OBJ_DATA(4,6);
		set_s_c(4,vec3(sphere_c(4) + vec3(0,0,-0.1)));
	}

//	if(pixel_coords.y == 0 && pixel_coords.x <= num_objs && obj_type(pixel_coords.x) == T_PARTICLE){
//		OBJ_SETV3(pixel_coords.x,4,vec3(sphere_c(pixel_coords.x) + OBJ_TOVEC3(pixel_coords.x,8)));
//	}



	//Corrisponding test-hack for data passthrough
//	colour = vec4(objs[pixel_coords.x],objs[pixel_coords.y],0,0);	
//	colour = vec4(1,0,0,1);

	

	if(ERROR ==0)
		imageStore(img_output, pixel_coords, colour);
	else
	  	error_out(pixel_coords);
}

