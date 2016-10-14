// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 textureCoords;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2DRect tex;

struct fData {
	int bw_mode;	// 0=none, 1= L1. ,2=L2, 3=L3, 4=sepia
	int conv_mode; // 0 = none, 1= sobel edge (horizontal), 2= sobel_edge(vertical) 3=sobel_edge(both, 4=unsharp, 5=gaussian 3x3, 6=gaussian 5x5, 7=gaussian 7x7.
};

uniform fData fragData;
float data;
	float sigma=0.6f; //FOR DEBUG MODE
float e = 2.7182818284f;

int x; int y;

void gaussian(){

//	//Hard coded 2D gaussian formula   DOESN'T SEEM TO WORK.
//	float sigma;
//	if(fragData.conv_mode == 5){
//		sigma=0.6f;
	float[3] matrix=float[3](0.2,0.6,0.2);
//	}else if(fragData.conv_mode == 6){
//		sigma=1.f;
//		float[5] matrix=float[5](0.06,0.24,0.4,0.24,0.06);
//	}else if(fragData.conv_mode == 7){
//		//sigma=1.4;
//		float[7] matrix=float[7](0.03,0.11,0.22,0.28,0.22,0.11,0.03);
//	}
//	float s2 = sigma*sigma;
//	float a = ((2.f*3.14159f)*s2);
//	float b = pow(e,-((x*x+y*y)/2.f*s2));
//	return(b/a);
//	data = (1.f/(2.f*3.14159f*sigma*sigma))*pow(e,-((x*x+y*y)/2.f*sigma*sigma));

	data = (matrix[x] * matrix[y]);
	
}

vec4 convolv(vec2 coords){	
	vec4 colour=vec4(0);
//	colour = texture(tex,coords+vec2(0,0));  

	int xsize=3;
	int ysize=3;
	
	int debug=1;
	
//	if(fragData.conv_mode == 5 || fragData.conv_mode == 6 || fragData.conv_mode == 7 || debug==1){
//		if(fragData.conv_mode == 6){xsize=5;ysize=5;}
//		else if(fragData.conv_mode == 7){xsize=7;ysize=7;} //Sets the required matrix sizes



//	if(fragData.conv_mode == 5){
//		sigma=0.6f;
//		float[3] matrix=float[3](0.2,0,1.9);
//	}else if(fragData.conv_mode == 6){
//		sigma=1.f;
//		float[5] matrix=float[5](0.06,0.24,0.4,0.24,0.06);
//	}else if(fragData.conv_mode == 7){
//		//sigma=1.4;
//		float[7] matrix=float[7](0.03,0.11,0.22,0.28,0.22,0.11,0.03);
//	}
	int r; int c;
	int ri; int ci;
	
		for(r= -(ysize/2) ; r < (ysize/2) ; r++){    	
			for(c= -(xsize/2) ; c < (xsize/2) ; c++){  	
				ri = (ysize/2) + r; 
				ci = (xsize/2) + c;
//				float data = gaussian(r,c,xsize); //THIS DOES NOT WORK. 
//				data *= 5.f;
//				data = 1f;
				x=r; y=c;
				gaussian();
//				data = (1.f/(2.f*3.14159f*sigma*sigma))*pow(e,-((x*x+y*y)/2.f*sigma*sigma));
//				data *= 2.f;
				colour += data*texture(tex,coords);
			}
		}

//	}


	colour[3]=0;
	return(colour);
}

void main(void){
    float res = 1f;
    vec2 newCoords;
    newCoords.x = textureCoords.x;
    newCoords.y = textureCoords.y;

	vec4 colour=vec4(0);
//	if(fragData.conv_mode == 0){
//		colour = texture(tex,newCoords+vec2(0,0));  
//		colour = vec4(0);
//	}else{
		colour = convolv(newCoords);		//Convolv if needed.
//	}

	if(fragData.bw_mode == 1){
		float lum;				//L = 1/3R + 1/3G + 1/3B
		lum = texture(tex, newCoords)[0]*	0.33333f; //RED
		lum += texture(tex, newCoords)[1]*	0.33333f; //GREEN
		lum += texture(tex, newCoords)[2]*	0.33333f; //BLUE
		colour.xyz=vec3(lum);
	
	}else if(fragData.bw_mode == 2){
		float lum;				//L = 0.299 R + 0.587 G + 0.114 B
		lum = texture(tex, newCoords)[0]*	0.299f; //RED
		lum += texture(tex, newCoords)[1]*	0.587f; //GREEN
		lum += texture(tex, newCoords)[2]*	0.114f; //BLUE
		colour.xyz=vec3(lum);
	
	}else if(fragData.bw_mode == 3){
		float lum;				//L = 0.213 R + 0.715 G + 0.072 B
		lum = texture(tex, newCoords)[0]*	0.213f; //RED
		lum += texture(tex, newCoords)[1]*	0.715f; //GREEN
		lum += texture(tex, newCoords)[2]*	0.072f; //BLUE
		colour.xyz=vec3(lum);
	}else if(fragData.bw_mode == 4){
		//SEPIA  : http://www.techrepublic.com/blog/how-do-i/how-do-i-convert-images-to-grayscale-and-sepia-tone-using-c/
			/*	outputRed = (inputRed * .393) + (inputGreen *.769) + (inputBlue * .189)
				outputGreen = (inputRed * .349) + (inputGreen *.686) + (inputBlue * .168)
				outputBlue = (inputRed * .272) + (inputGreen *.534) + (inputBlue * .131)
			*/
		float r=texture(tex,newCoords)[0];
		float g=texture(tex,newCoords)[1];
		float b=texture(tex,newCoords)[2];
		colour.x = r*.393f	+ g*.769f	+ b*.189f ;
		colour.y = r*.349f	+ g*.686f	+ b*.168f ;
		colour.z = r*.272f	+ g*.534f	+ b*.131f ;
	}
    FragmentColour = colour;
}

