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


#define OBJSIZE 21
#define T_NONE -7
#define T_TRI 1
#define T_LIGHT 2
#define T_SPHERE 3
#define T_PLANE 4
#define T_POINT 5
#define T_PARTICLE 6

////////////////////////////////////////Accsessor functions for objects.

#define OBJ objs
//Offset by 1, for the object[] size.
#define num_objs int(OBJ[0])
//Offset by 3, for ambient light levels.
#define ambient vec3(OBJ[1],OBJ[2],OBJ[3])

#define OBJ_BADDR(X) ((X*OBJSIZE)+4) 
#define OBJ_ADDR(X,Y) OBJ_BADDR(X)+Y
#define OBJ_DATA(X,Y) OBJ[OBJ_ADDR(X,Y)]
#define OBJ_TOVEC3(X,Y) vec3(OBJ_DATA(X,Y),OBJ_DATA(X,Y+1),OBJ_DATA(X,Y+2))
#define OBJ_SETV3(X,Y,Z)  OBJ_DATA(X,Y)=Z.x; OBJ_DATA(X,Y+1)=Z.y; OBJ_DATA(X,Y+2)=Z.z;

#define obj_type(X) 		OBJ_DATA(X,0)
#define obj_colour(X) 		OBJ_TOVEC3(X,1)
#define obj_pcolour(X) 		OBJ_TOVEC3(X,4)
#define obj_velc(X)   		OBJ_TOVEC3(X,7)
#define obj_phong(X)   		OBJ_DATA(X,10)
#define obj_reflec(X)		OBJ_DATA(X,11)
#define OBJ_LOCD		12

#define OBJ_DATAL(X,Y)		OBJ_DATA(X,(OBJ_LOCD+Y))
#define OBJ_TOVEC3L(X,Y)	OBJ_TOVEC3(X,(OBJ_LOCD+Y))
#define OBJ_SETV3L(X,Y,Z)	OBJ_SETV3(X,(OBJ_LOCD+Y),Z)

#define sphere_c(X) OBJ_TOVEC3L(X,0)
#define sphere_r(X) OBJ_DATAL(X,3)
#define sphere_setc(X,Y) OBJ_SETV3L(X,0,Y)

#define tri_p1(X) OBJ_TOVEC3L(X,0)
#define tri_p2(X) OBJ_TOVEC3L(X,3)
#define tri_p3(X) OBJ_TOVEC3L(X,6)

#define plane_n(X) OBJ_TOVEC3L(X,0)
#define plane_p(X) OBJ_TOVEC3L(X,3)
#define plane_ned(X) OBJ_DATAL(X,6)

#define light_p(X) OBJ_TOVEC3L(X,0)

#define get_rray ref_rays[(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y*(imageSize (img_output)).x)]

#define PI 3.141592653587939
#define EPSILON 0.000001
#define BEPSILON 0.001
#define FUDGE 0.001

struct ray{
	vec3 origin;
	vec3 direction;
};


layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

layout(std430, binding = 1) buffer object_buffer{
	float objs[];
};

uniform float FOV;
uniform vec3 offset;
uniform mat3 transform;

//vec4 colour = vec4(0);
bool check_light_hit = false;		// Weither or not to check lights.
ray newray;			// A reflection ray.
int hitobj;			// Last-hit object.


//vec4 cam = vec4(0,0,0,PI/3); //


void obj_move(int obj){
	switch(int(obj_type(obj))){
		case T_TRI:
			//TODO	
			return;
		case T_PLANE:
			//TODO
			return;
		case T_SPHERE:
		case T_POINT:
		case T_LIGHT:
		case T_PARTICLE:
			vec3 pos = sphere_c(obj) + obj_velc(obj);
			sphere_setc(obj,pos);
			return; 
	}
}
int ERROR=0;
#define E_OTHER 1
#define E_TYPE 2


void set_error(int type){
	if(ERROR ==0)				//ONLY SET THE 1st ERROR.
		ERROR=type;
}
void error_out(ivec2 pixel_coords){	
	vec4 c = vec4(1,1,0,1);			//MAKE SCREEN YELLOW ON OBJECT ERROR
	imageStore(img_output, pixel_coords, c);
}

float ray_intersect_sphere(ray r, uint obj_ID){
	
	float t1,t2;
	float a = dot(r.direction,r.direction);
	float b = 2* dot(r.direction, (r.origin - sphere_c(obj_ID)))  ;
	float c = dot((r.origin - sphere_c(obj_ID)),(r.origin - sphere_c(obj_ID)))-pow(sphere_r(obj_ID),2);


	float det = (pow(b,2) - 4*a*c);
	if(det < EPSILON){return -1;}
	t1 = (-b + sqrt(det))/2*a;
	t2 = (-b - sqrt(det))/2*a;

	if(t1 < t2 && t1 > 0){
		return(t1);
	}else
		return(t2);
		
}
#define TEST_CULL
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
	vec3 tvec = r.origin - tri_p1(obj);
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

//	if(plane_ned(obj)==0 && ivec2(gl_GlobalInvocationID.xy) == ivec2(0)){		//Probably causes some error on 1st frame.
//		OBJ_SETV3(obj,3,normalize(plane_n(obj)));	
//		plane_ned(obj) = 1;
//	}
	vec3 norm = normalize(plane_n(obj));

	float d = dot(r.direction,norm);
	if(d < EPSILON && d > -EPSILON)
		return -1;
	float t = dot(norm,(plane_p(obj)-r.origin));
	t /= d;

	if(t <= EPSILON)
		return -1;

	return t;

}

float ray_intersect_point(ray r, uint obj){
	vec3 t= (-r.origin + light_p(obj));
	t.x /= r.direction.x;
	t.y /= r.direction.y;
	t.z /= r.direction.z;

	//Approxmation, ends up very poor.

	if(abs(t.x - t.y) < BEPSILON || abs(r.direction.x) < BEPSILON || abs(r.direction.y) < BEPSILON ){
		if(abs(t.x -t.z ) < BEPSILON || abs(r.direction.x) < BEPSILON || abs(r.direction.z) < BEPSILON ){
			if(abs(t.y - t.z) < BEPSILON || abs(r.direction.y) < BEPSILON || abs(r.direction.z) < BEPSILON ){
				return(min(t.x,min(t.y,t.z)));
			}
		}
	} 
	return -1;


	if(abs(t.x - t.y)< BEPSILON && abs(t.x - t.z) < BEPSILON && abs(t.y - t.z) < BEPSILON){
		return t.x;
	} else
		return -1;
	

}

float test_object_intersect(ray r, uint obj){
	switch(int(obj_type(obj))){
		case T_TRI:
			return ray_intersect_triangle(r,obj);	
		case T_SPHERE:
			return ray_intersect_sphere(r,obj);
		case T_PLANE:
			return ray_intersect_plane(r,obj);
		case T_POINT:
			return(-1);
			//return ray_intersect_point(r,obj);
		case T_LIGHT:
			if(check_light_hit==false)
				return(-1);
			else
				return ray_intersect_point(r,obj);
		case T_PARTICLE:
			return ray_intersect_sphere(r,obj);
	}
	//If we get here, we have issues.
	set_error(E_TYPE);
}

vec4 test_objects_intersect(ray r){ //Tests _ALL_ objects
	float t;
	vec4 ret;
	ret.x = -1;
	ret.y = T_NONE;
	int i = 0;
	for(i=0; i < num_objs ; i++ ){
		t = test_object_intersect(r,i);
		if( ( ret.x < 0 ||  t < ret.x ) && t > 0  ){
			ret.x = t; //Distance
			ret.y = i; //Object
		}		
	}
	return(ret);
}

vec3 get_surface_norm(vec3 hitpos, uint obj){
	switch(int(obj_type(obj))){
		case T_TRI:
			vec3 tmp = normalize(cross(tri_p1(obj)-tri_p2(obj),tri_p1(obj)-tri_p3(obj)));
			return(tmp);
		case T_SPHERE:
			return normalize(hitpos-sphere_c(obj));
		case T_PLANE:
			return normalize(plane_n(obj));
		case T_POINT:
			return vec3(0);
		case T_LIGHT:
			return vec3(0);
		case T_PARTICLE:
			return normalize(hitpos-sphere_c(obj));
		case T_NONE:
			return vec3(0);

	}
}


vec4 rtrace(ray cray){

	vec4 c=vec4(0);
	//////////////////BASIC RAY-TRACING///////////////////
	check_light_hit=false;
	vec4 res = test_objects_intersect(cray);
	if(res.x >= 0)
		c = vec4(obj_colour(int(res.y)),0);

	vec3 hitpos = res.x * cray.direction + cray.origin;
	vec3 surface_norm  = get_surface_norm(hitpos, int(res.y));	
	
	/////////////////////////////UPDATE GLOBALS FOR REFLECTIONS//////////////

	hitobj = int(res.y);
	newray.direction = normalize(reflect(cray.direction, surface_norm));
	newray.origin = hitpos + newray.direction*FUDGE;
	
	///////////////////BASIC SHADOWS////////////////////////////////
	//Iterates through each object to find lights, making check_light_hit rays and testing them as it goes.
	int lcnt=0;
	int scnt=0;
	vec4 stest;
	ray sray;
	check_light_hit = true;
	vec3 svect;
	float svlen;

	#ifdef hard_shadows
	
	for(int i=0; i < num_objs ; i++){
		if(obj_type(i) == T_LIGHT){
			lcnt++;
			svect = light_p(i) - hitpos;
			svlen = sqrt(dot(svect,svect));

			sray.direction = normalize(svect);
			sray.origin = hitpos + sray.direction * FUDGE;

			stest = test_objects_intersect(sray);

			if(int(obj_type(int(stest.y))) == T_LIGHT){				 //If it intersects a light.
				scnt++;
			}
			
		}
	}
//	if(scnt = 0)
//		c *= ((lcnt-scnt)/lcnt);		//Apply check_light_hits.
	c *= (lcnt/scnt);

	return(c);
	
	#else
	////////////////////Diffuse Lights/////////////////////


	vec4 c_scaler = vec4(ambient,0);	//Set-up minimum of ambient.
	vec4 p_scaler = vec4(0);
	
	check_light_hit = true;
	if(hitobj != T_NONE && (1-obj_reflec(hitobj)) >= BEPSILON){
	for(int i=0; i < num_objs ; i++){
	if(obj_type(i) == T_LIGHT){
		svect = light_p(i) - hitpos;
		svlen = sqrt(dot(svect,svect));
		sray.direction = normalize(svect);

		sray.origin = hitpos + sray.direction *FUDGE;
		stest = test_objects_intersect(sray);

		if((int(obj_type(int(stest.y))) == T_LIGHT)){
			//DIFFUSE
			c_scaler += vec4((obj_colour(i)*max(0,dot(sray.direction,surface_norm))),0);
			//PHONG
			vec3 h = normalize((-cray.direction + sray.direction));
			float a = max(0,(dot(h,surface_norm)));
			p_scaler += vec4( pow(a,obj_phong(hitobj)) * obj_colour(i)  ,0  );

		}
		}
	}
	}
	
	c = (c * vec4(c_scaler)) + vec4(p_scaler);
	return(c);
	#endif
}


void main(){
	
  	ivec2 dims = imageSize (img_output);

  	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

  	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	
	vec4 colour = vec4(0);

	vec2 coords = vec2(gl_GlobalInvocationID.xy);
	coords = coords / vec2(dims.xy/2);
	coords = coords + vec2(-1);


	//BASIC	pos
	newray.direction = vec3(coords, -1/tan(FOV/2));	
	newray.origin = vec3(0,0,0);	

	//TRANSFORMATIONS
	newray.origin = offset;
	newray.direction = transform*newray.direction;

	newray.direction = normalize(newray.direction);

	/////////////////INIT////////////////////////////////
	colour =vec4(0);
	vec4 c2;
	float ref_pwr=1;


//#define stack_reflect 10

#ifndef stack_reflect
	//Iterative Reflection
//	#define reflect_by_num 2

	#ifdef reflect_by_num
	for(int i=0; i < reflect_by_num ; i ++){	//By fixed ammount
	#else
	while(ref_pwr >= BEPSILON){			//Dynamicly
	#endif
		c2 = rtrace(newray);			
		colour += c2 * (ref_pwr) * (1-obj_reflec(hitobj));
		ref_pwr*=obj_reflec(hitobj);
		if(hitobj == T_NONE)
			break;
	}
		
#else
	//Stack based-reflection. (Way slower)
	vec4 Cstack[stack_reflect];

	for(int i=0; i < stack_reflect ; i++){
		Cstack[i]=vec4(rtrace(newray).xyz,obj_reflec(hitobj));
	}
	for(int i=stack_reflect-1 ; i >= 0 ; i--){
		colour.xyz =  mix(Cstack[i].xyz, colour.xyz, Cstack[i].w); 
	}
	
#endif

	//MOVES PARTICLES. NOTE: NO COLLISONS YET.
	if(pixel_coords.y == 0 && pixel_coords.x <= num_objs && obj_type(pixel_coords.x) == T_PARTICLE){
		obj_move(pixel_coords.x);
	}
	if(ERROR ==0)
		imageStore(img_output, pixel_coords, colour);
	else
	  	error_out(pixel_coords);
}

