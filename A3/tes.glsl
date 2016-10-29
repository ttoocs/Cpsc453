//TES transforms new vertices. Runs once for all output vertices

#version 410
layout(isolines) in; // Controls how tesselator creates new geometry

in vec3 teColour[]; // input colours

out vec3 Colour; // colours to fragment shader


//uniform int beziercurve_level = 1;
int level = int(teColour[4].x);

#define co_length 4 //Length of an entry
#define co_entires 4	//Number of entries

//Ideally this should be a shader buffer object that calculates itself as needed, but for this will do.
int coeffish[4*4] = int[](
	1, 0, 0, 0,
	1, 1, 0, 0,
	1, 2, 1, 0,
	1, 3, 3, 1
);

void main()
{
    int i=0;
    float u = gl_TessCoord.x;

	vec4 pos=vec4(0,0,0,0);
	vec3 col=vec3(0,0,0);
	float b[4];

	b[0] = pow((1-u),level) 	* pow(u,i);
	b[1] = pow((1-u),level -1) 	* pow(u,i+1);
	
	for(i=0; i<=level; i++){
		float p1 = pow((1-u),(level-i));
		float p2 = pow(u,i);
		if(u ==0 && i==0){ p2=1;}
		if(u ==1 && i==level){ p1=1;}			//Handels edge cases of u=0 or 1

		float bn= p1*p2;  //Beizer curve coeffishent
		bn *= coeffish[level*4 + i];						//The binomial coeffishent
		b[i] = bn;
		pos = pos + bn*gl_in[i].gl_Position;				//Position
		col = col + bn*teColour[i];						//Colour.
	}
	


	gl_Position = pos;

//	gl_Position = b[0]*gl_in[0].gl_Position + b[1]*gl_in[1].gl_Position + b[2]*gl_in[2].gl_Position + b[3]*gl_in[3].gl_Position;

	Colour = col;

//    Colour 	= ((1-u) * teColour[0]) +  (u * teColour[i]);
//    Colour 	= ((1-u) * vec3(1,1,0)) +  (u * vec3(0,0,1));
//	  Colour = vec3(1,1,1) * pow(1,0);

}



