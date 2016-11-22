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
#define T_NONE 7
#define T_TRI 1
#define T_LIGHT 2
#define T_SPHERE 3
#define T_PLANE 4
#define T_POINT 5
#define T_PARTICLE 6

//Accsessor functions for objects.

#define OBJ objs
//Offset by 1, for the object[] size.
#define num_objs int(OBJ[0])
//Offset by 3, for ambient light levels.
#define ambient vec3(OBJ[1],OBJ[2],OBJ[3])

#define OBJ_BADDR(X) ((X*OBJSIZE)+4) 
#define OBJ_ADDR(X,Y) OBJ_BADDR(X)+Y
#define OBJ_DATA(X,Y) OBJ[OBJ_ADDR(X,Y)]
#define OBJ_TOVEC3(X,Y) vec3(OBJ_DATA(X,Y),OBJ_DATA(X,Y+1),OBJ_DATA(X,Y+2))
#define OBJ_SETV3(X,Y,Z)  OBJ_DATA(X,Y)=Z.x; OBJ_DATA(X,Y)=Z.y; OBJ_DATA(X,Y)=Z.z;

#define obj_type(X) 		OBJ_DATA(X,0)
#define obj_colour(X) 		OBJ_TOVEC3(X,1)
#define obj_pcolour(X) 		OBJ_TOVEC3(X,4)
#define obj_velc(X)   		OBJ_TOVEC3(7)
#define obj_phong(X)   		OBJ_DATA(X,10)
#define obj_reflec(X)		OBJ_DATA(X,11)
#define OBJ_LOCD		12

#define OBJ_DATAL(X,Y)		OBJ_DATA(X,(OBJ_LOCD+Y))
#define OBJ_TOVEC3L(X,Y)	OBJ_TOVEC3(X,(OBJ_LOCD+Y))

#define sphere_c(X) OBJ_TOVEC3L(X,0)
#define sphere_r(X) OBJ_DATAL(X,3)

#define tri_p1(X) OBJ_TOVEC3L(X,0)
#define tri_p2(X) OBJ_TOVEC3L(X,3)
#define tri_p3(X) OBJ_TOVEC3L(X,6)

#define plane_n(X) OBJ_TOVEC3L(X,0)
#define plane_p(X) OBJ_TOVEC3L(X,3)
#define plane_ned(X) OBJ_DATAL(X,6)

#define light_p(X) OBJ_TOVEC3L(X,0)

#define get_rray ref_rays[(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y*(imageSize (img_output)).x)]

#define PI 3.14159265358793
#define EPSILON 0.000001

//#define ssbo_ref

struct ray{
	vec3 origin;
	vec3 direction;
};

#ifdef ssbo_ref
struct RefRay{
	ray 	r;
	vec3 	data;
};
#endif

vec4 colour = vec4(0);
bool shadow = false;

//uniform mat4x2  cam = mat4x2(0,0,0,PI/3,0,0,0,0);
vec4 cam = vec4(0,0,0,PI/3);

layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

layout(std430, binding = 1) buffer object_buffer{
	float objs[];
};

#ifdef ssbo_ref
layout(std430, binding = 2) buffer reflection_buffer{
	vec4 ref_state;
	RefRay ref_rays[];
};
#endif 

int ERROR=0;
#define E_OTHER 1
#define E_TYPE 2
void set_error(int type){
	if(ERROR ==0)
		ERROR=type;
}
void error_out(ivec2 pixel_coords){	
	vec4 c = vec4(1,1,0,1);
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
	float b = 2* dot(r.direction, (r.origin - sphere_c(obj_ID)))  ;
	float c = dot((r.origin - sphere_c(obj_ID)),(r.origin - sphere_c(obj_ID)))-pow(sphere_r(obj_ID),2);


	float det = (pow(b,2) - 4*a*c);
	if(det < EPSILON){return -1;}
	t1 = (-b + sqrt(det))/2*a;
	t2 = (-b - sqrt(det))/2*a;

	if(t1 < t2 && t1 >= 0){
		return(t1);
	}else
		return(t2);
		
}
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

#define BEPSILON 0.01
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

// unifrom float objets[];

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
			if(shadow==false)
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
		if( ( ret.x < 0 ||  t < ret.x ) && t >= 0  ){
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
			return vec3(0);
		case T_NONE:
			return vec3(0);

	}
}

#ifndef ssbo_ref
ray newray;
int hitobj;
#endif

vec4 rtrace(ray cray){

	//////////////////BASIC RAY-TRACING///////////////////
	shadow=false;
	vec4 res = test_objects_intersect(cray);
	if(res.x >= 0)
		colour = vec4(obj_colour(int(res.y)),0);

	vec3 hitpos = res.x * cray.direction + cray.origin;

	//UPDATE THINGS FOR FUTURE USE./////////////////////////////////

	vec3 surface_norm  = get_surface_norm(hitpos, int(res.y));	
	#ifdef ssbo_ref
	get_rray.r.origin = hitpos;
	get_rray.r.direction = reflect(cray.direction, surface_norm);
	get_rray.data.x *= obj_reflec(int(res.y));
	#else
	hitobj = int(res.y);
	newray.direction = normalize(reflect(cray.direction, surface_norm));
	newray.origin = hitpos + newray.direction*0.01;
	#endif
	
	///////////////////BASIC SHADOWS////////////////////////////////
	//Iterates through each object to find lights, making shadow rays and testing them as it goes.
	int lcnt=0;
	int scnt=0;
	vec4 stest;
	ray sray;
	shadow = true;
	vec3 svect;
	float svlen;
	for(int i=0; i < num_objs ; i++){
		if(obj_type(i) == T_LIGHT){
			lcnt++;
			svect = light_p(i) - hitpos;
			svlen = sqrt(dot(svect,svect));

			sray.direction = normalize(svect);
			sray.origin = hitpos + sray.direction *0.01;

			stest = test_objects_intersect(sray);

			if(int(obj_type(int(stest.y))) != T_LIGHT){				 //If it intersects a light.
				scnt++;
			}
			
		}
	}
	if(scnt != 0)
		colour *= ((lcnt-scnt)/lcnt);		//Apply shadows.
//		colour *= 0.2;
	////////////////////Diffuse Lights/////////////////////

	shadow = true;
	if(scnt == 0){	//If not in shadow
	int cnt;
	for(int i=0; i < num_objs ; i++){
	if(obj_type(i) == T_LIGHT){
		cnt++;
		svect = light_p(i) - hitpos;
		svlen = sqrt(dot(svect,svect));
		sray.direction = normalize(svect);

		sray.origin = hitpos + sray.direction *0.01;
		stest = test_objects_intersect(sray);

		if((int(obj_type(int(stest.y))) == T_LIGHT)){
//			vec3 surface_norm  = get_surface_norm(hitpos, int(res.y));	//Get _a_ normal

//			colour = vec4(surface_norm,0);		//Display the norm
			//DIFFUSE
			colour = colour * vec4((ambient + obj_colour(i)*max(0,dot(sray.direction,surface_norm))),0);
			//PHONG
			vec3 h = (-cray.direction + sray.direction);
			h /= length(h);
			colour += vec4(obj_colour(i)*obj_pcolour(int(res.y))*pow(max(0,dot(surface_norm,h)),obj_phong(i)),0);

		}
		}
	}
	}


	return(colour);
}
void main(){
	
//	vec4 cam = vec4(0,0,0,PI/3);

  	ivec2 dims = imageSize (img_output);

  	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

  	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	
	vec4 colour = vec4(0);

	vec2 coords = vec2(gl_GlobalInvocationID.xy)/256;
	coords = coords + vec2(-1);

	//Red-green-ness	
//	colour = vec4(abs(coords),0,0);
//	colour = (vec4(ray_intersect_sphere(cray,0)!=-1));
//	colour = (vec4(ray_intersect_triangle(cray,1)!=-1));	//Test ray intersects (scene3)
//	colour = (vec4(1/(ray_intersect_plane(cray,2))));

	/////////////////REFLECTIONS////////////////////////////////
	/// 
	// Check: The coeffishent is in get_rray.data.x
	//		Update said coeffishent as needed
	//		
	//		Add to colour

	#ifdef ssbo_ref
//	colour = vec4(abs(ref_state.w/1));	
//	if(ref_state.w == 0){	//NOT ALWAYS SET TO ZERO?
		get_rray.data.x = 1;
		get_rray.data.y = 0;	//Not used for anything.
		get_rray.data.z = 0;

		ray cray;
		cray.origin = vec3(cam.xyz);
		cray.direction = vec3(coords, -1/tan(cam.w/2));
		cray.direction = normalize(cray.direction);

		colour = rtrace(cray) * (1-get_rray.data.x); //IS GOOD.
		ref_state.w = 1;
//	}
		
//	else if(ref_state.w == 1){
//		colour = imageLoad(img_output, pixel_coords); //Load a colour.
//		ref_state.w++;
//		vec4 cnew = rtrace(get_rray.r);
//		colour = vec4(get_rray.r.origin,0);
//		colour += cnew * (1-get_rray.data.x);  
//	}
//	else
//		ref_state.w++;
	#else 
	//Because the ideal system of slowly adding more reflections is failing, a simpler system is needed.
	colour =vec4(0);
	vec4 c2;
	float ref_pwr=1;

	newray.direction = vec3(coords, -1/tan(cam.w/2));
	newray.origin = vec3(cam.xyz);
//	newray.origin = vec3(cam[0][0],cam[0][1],cam[0][2]);
//	newray.direction = vec3(coords, -1/tan(PI/6));	//INITAL SETUP
//	newray.direction = //TRANSPOSE THE LOOK-DIR
	newray.direction = normalize(newray.direction);

//#define stack_reflect 10

#ifndef stack_reflect

//#define reflect_by_num 10
	#ifdef reflect_by_num
	for(int i=0; i < reflect_by_num ; i ++){
	#else
	while(ref_pwr >= EPSILON){
	#endif
//		c2 = rtrace(newray);
//		ref_pwr*=obj_reflec(hitobj);				//Attempt at recursion-less.
//		colour = (colour - c2*ref_pwr)/(1-ref_pwr);
		
		c2 = rtrace(newray);			//Works
		ref_pwr*=obj_reflec(hitobj);
		colour += c2 * (1-ref_pwr);

//		if(hitobj == T_NONE)
//			break;			//Quit checking if we hit nothing.
	}
		
//	colour *= 0.5;
		
#else
	//Stack based-recursion.
	vec4 Cstack[stack_reflect];
	float Rstack[stack_reflect];

	for(int i=0; i < stack_reflect ; i++){
		Cstack[i]=rtrace(newray);
		Rstack[i]=obj_reflec(hitobj);
	}
	for(int i=stack_reflect-1 ; i >= 0 ; i--){
		colour =  mix(Cstack[i], colour, Rstack[i]); 
	}
	
#endif

#endif
	//MOVES PARTICLES. NOTE: NO COLLISONS YET.
//	if(pixel_coords.y == 0 && pixel_coords.x <= num_objs && obj_type(pixel_coords.x) == T_PARTICLE){
//		OBJ_DATA(pixel_coords.x,4) += OBJ_DATA(pixel_coords.x,8);
//		OBJ_DATA(pixel_coords.x,5) += OBJ_DATA(pixel_coords.x,9);
//		OBJ_DATA(pixel_coords.x,6) += OBJ_DATA(pixel_coords.x,10);
//		OBJ_SETV3(pixel_coords.x,4,vec3(sphere_c(pixel_coords.x) + OBJ_TOVEC3(pixel_coords.x,8)));
//	}

	//Corrisponding test-hack for data passthrough
//	colour = vec4(objs[pixel_coords.x],-objs[pixel_coords.y],0,0)/5;	
//	colour = vec4(1,0,0,1);

	if(ERROR ==0)
		imageStore(img_output, pixel_coords, colour);
	else
	  	error_out(pixel_coords);
}

