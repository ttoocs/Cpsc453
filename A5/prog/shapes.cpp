

#define torad(X)  ((float)(X*PI/180.f))

#define V_PUSH(X,a,b,c) X.push_back(vec3(a,b,c)); //Re-wrttien for GLM.

#define P_indices(X,a,b,c)	      X.push_back(a); X.push_back(b); X.push_back(c);


typedef struct TextureStuff;
struct TextureStuff{
	int components;	
	int tWidth;
	int tHeight;
	unsigned char* data=NULL;

};

typedef struct Object Object; 
struct Object{ 
  std::vector<glm::vec3> positions; 
  std::vector<glm::vec3> normals;     //What are these used for? -- Lighting. 
  std::vector<glm::vec2> uvs;     //My guess this is for textures? 
  std::vector<unsigned int> indices; 
	TextureStuff texture;
	glm::mat4 modelview=mat4(1.f);
	
};



void nukeshape(Object *obj){
	obj->positions.clear();
	obj->normals.clear();
	obj->uvs.clear();
	obj->indices.clear();
	obj->modelview=mat4(1.f);
}

void loadTexture(Object *obj, const char* filename){

  obj->texture.data = stbi_load(filename, &(obj->texture.tWidth), &(obj->texture.tHeight), &(obj->texture.components), 0);

}

void moveObj(Object *obj, glm::vec3 pos){
	obj->modelview[3][0] += pos[0]; 
	obj->modelview[3][1] += pos[1]; 
	obj->modelview[3][2] += pos[2]; 
}
/*void moveObjPos(Object *obj, glm::vec3 pos){
	moveObj(obj,pos);
} */

void rotateObj(Object *obj,vec3 axis, float radians){
	mat4 trans = rotateAbout(axis,radians);

	//Via altering modelview
	obj->modelview = trans* obj->modelview;
//	//via changing positions.
//	for(int i=0; i< obj->positions.size(); i++){ //Does 
//		obj->positions.data()[i] = trans*glm::vec4(obj->positions.data()[i],1);
//	}
}
void rotateObjPos(Object *obj,vec3 axis, float radians){
	mat4 trans = rotateAbout(axis,radians);

	//Via altering modelview
//	obj->modelview = trans* obj->modelview;
	//via changing positions.
	for(int i=0; i< obj->positions.size(); i++){ //Does 
		obj->positions.data()[i] = trans*glm::vec4(obj->positions.data()[i],1);
	}
}

void generateTri(Object *obj){	//Currently used just for debugging.
	nukeshape(obj);
	V_PUSH(obj->positions,0,0,0);
	V_PUSH(obj->positions,0,1,0);
	V_PUSH(obj->positions,1,1,0);
	
	V_PUSH(obj->normals,1,0,0);
	V_PUSH(obj->normals,0,1,0);
	V_PUSH(obj->normals,0,0,1);

	obj->indices.push_back(0);
	obj->indices.push_back(1);
	obj->indices.push_back(2);


}	


//u parameterizes in the big circle, v parameterizes in the little circle
//c_r is the circle radius, t_r is the tube radius
void generateTorus(Object * obj, float c_r, float t_r, int uDivisions, int vDivisions)
{
	nukeshape(obj);
  float uStep = 1.f/(float)(uDivisions-1);
  float vStep = 1.f/(float)(vDivisions-1);

  float u = 0.f;

  //Traversing u
  for(int i=0; i<uDivisions; i++)
  {

    vec3 center = vec3( c_r*cos(2.f*PI*u), 0.f, c_r*sin(2.f*PI*u));

    float v = 0.f;

    //Traversing v
    for(int j=0; j<vDivisions; j++)
    {
      vec3 pos = vec3(  (c_r+t_r*cos(2.f*PI*v)) * cos(2.f*PI*u),
                t_r*sin(2.f*PI*v),
                (c_r+t_r*cos(2.f*PI*v)) * sin(2.f*PI*u));
//            vec3 pos = vec3(c_r*cos(2.f*PI*u)*sin(2.f*PI*v),
//                      c_r*sin(2.f*PI*u)*cos(2.f*PI*v),
//                      c_r*cos(2.f*PI*v));



      vec3 normal = normalize(pos - center);

      obj->positions.push_back(pos);
      obj->normals.push_back(normal);
      obj->uvs.push_back(vec2(u, v));


      v += vStep;
    }
  
    u += uStep;
  }

  for(int i=0; i<uDivisions-1; i++)
  {
    for(int j=0; j<vDivisions -1; j++)
    {
      unsigned int p00 = i*vDivisions+j;
      unsigned int p01 = i*vDivisions+j+1;
      unsigned int p10 = (i+1)*vDivisions + j;
      unsigned int p11 = (i+1)*vDivisions + j + 1;
    
      obj->indices.push_back(p00);
      obj->indices.push_back(p10);
      obj->indices.push_back(p01);

      obj->indices.push_back(p01);
      obj->indices.push_back(p10);
      obj->indices.push_back(p11);
    }
  }
}   


void generateSphere(Object * obj, float radius, int udiv, int vdiv){
	nukeshape(obj);
	
	//Generate Positions:
	float uStep = 1.f/(float)(udiv-1);
	float vStep = 1.f/(float)(vdiv-1);

	#define uScale  (u*PI)
	#define vScale  (v*2.f*PI)

	float u=0.f;
	for(int i=0; i < udiv; i++){ //u
		float v = 0.f;
		for(int j=0; j < vdiv; j++){ //v

			glm::vec3 pos = glm::vec3(
											radius*sin(uScale)*cos(vScale),
											radius*sin(uScale)*sin(vScale),
											radius*cos(uScale)
											);			

			obj->positions.push_back(pos);
			obj->normals.push_back(pos);		//Normal of a sphere at origin is just the pos.
			obj->uvs.push_back(glm::vec2(v,u));
			
			v+=vStep;
		}
		u+=uStep;
	}

	#undef uScale
	#undef vScale
	
	//Put indicies
	for(int i=0; i < udiv-1; i++){ //u
		for(int j=0; j < vdiv-1; j++){ //v
					unsigned int p00 = i*vdiv+j;
					unsigned int p01 = i*vdiv+j+1;
					unsigned int p10 = (i+1)*vdiv + j;
					unsigned int p11 = (i+1)*vdiv + j + 1;
					//p00 = this,
					//p01 = "right"
					//p10 = "down"
					//p11 = "right+down"
				
					P_indices(obj->indices,p00,p01,p11);					
					P_indices(obj->indices,p00,p10,p11);					


		}
	}


}


