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
	int bw_mode;		// 0=none, 1= L1, 2=L2, 3=L3, 4=sepia
	int conv_mode;  	// 0=none, 1= sobel edge (horizontal), 2= sobel_edge(vertical)
						// 3=sobel_edge(both, 4=unsharp, 5=gaussian 3x3,
						// 6=gaussian 5x5, 7=gaussian 7x7.
};

uniform fData fragData;

#define e 2.7182818284f
#define pi 3.1415926534

float gaussian1d(int x, float s){
    float ret;
    ret=(1.f)/sqrt(2*pi*s*s);
    ret *= pow(e,(-x*x)/(2*s*s));
    return(ret);
}
float gaussian2d(int x, int y, float sigma){
	return(gaussian1d(x,sigma)*gaussian1d(y,sigma));
}


vec4 conv_3x3(mat3 matrix, vec2 Cords){
	int row;
	int col;
	vec4 sum=vec4(0);
	for(row=0 ; row <= 2 ; row++){
		for(col=0 ; col <= 2; col++){
			sum += matrix[2-row][2-col]*texture(tex,Cords + vec2(row,col));
		}
	}
	return(abs(sum));
}

vec4 convolv(vec2 coords){
	vec4 colour=vec4(0);
	float sigma=1f;
	int xsize=0;
	int ysize=0;

	mat3 hor_edge=mat3(1,0,-1,2,0,-2,1,0,-1);
	mat3 vert_edge=mat3(1,2,1,0,0,0,-1,-2,-1);
	mat3 unsharp=mat3(0,-1,0,-1,5,-1,0,-1,0);
	mat3 gauss=mat3(0.04,0.12,0.04,0.12,0.36,0.12,0.04,0.12,0.04);	
//	colour = conv_3x3(gauss, newCoords);

	
	if(fragData.conv_mode == 5 || fragData.conv_mode == 6 || fragData.conv_mode == 7){
		if(fragData.conv_mode >= 5){
			int avar = fragData.conv_mode - 4;
			avar *= 2;
			avar++;
			xsize = avar;
			ysize = avar;
			sigma = avar/5.f;
		}

//		if(fragData.conv_mode == 5){xsize=3;ysize=3; 		sigma=0.6f;}
//		else if(fragData.conv_mode == 6){xsize=5;ysize=5;	sigma=1.0f;}
//		else if(fragData.conv_mode == 7){xsize=7;ysize=7;	sigma=1.4f;} //Sets the required matrix sizes

		int r; int c;
		int ri; int ci;
			for(r= 0; r <= ysize ; r++){    	
				for(c= 0 ; c < xsize ; c++){
					int x = xsize/2 - r;
					int y = ysize/2 - c;
					int cx = r - xsize/2;
					int rx = c - ysize/2;
					float data = gaussian2d(x,y,sigma);
					colour += (vec4(data*texture(tex,coords+vec2(rx,cx))));
				}
			}
	}
	colour[3]=0;
	return(colour);
}


void main(void){

	while(true){ 
		continue;
	}
	
    float res = 1f;
    vec2 newCoords;
    newCoords.x = textureCoords.x;
    newCoords.y = textureCoords.y;

	vec4 colour=vec4(0);

	if(fragData.conv_mode == 0){
		colour = texture(tex,newCoords+vec2(0,0));  
	}else{
		colour = convolv(newCoords);		//Convolv if needed.
	}

	if(fragData.bw_mode == 1){
		float lum;				//L = 1/3R + 1/3G + 1/3B
		lum = colour[0]*	0.33333f; //RED
		lum += colour[1]*	0.33333f; //GREEN
		lum += colour[2]*	0.33333f; //BLUE
		colour.xyz=vec3(lum);
	
	}else if(fragData.bw_mode == 2){
		float lum;				//L = 0.299 R + 0.587 G + 0.114 B
		lum = colour[0]*	0.299f; //RED
		lum += colour[1]*	0.587f; //GREEN
		lum += colour[2]*	0.114f; //BLUE
		colour.xyz=vec3(lum);
	
	}else if(fragData.bw_mode == 3){
		float lum;				//L = 0.213 R + 0.715 G + 0.072 B
		lum = colour[0]*	0.213f; //RED
		lum += colour[1]*	0.715f; //GREEN
		lum += colour[2]*	0.072f; //BLUE
		colour.xyz=vec3(lum);
	}else if(fragData.bw_mode == 4){
		//SEPIA  : http://www.techrepublic.com/blog/how-do-i/how-do-i-convert-images-to-grayscale-and-sepia-tone-using-c/
			/*	outputRed = (inputRed * .393) + (inputGreen *.769) + (inputBlue * .189)
				outputGreen = (inputRed * .349) + (inputGreen *.686) + (inputBlue * .168)
				outputBlue = (inputRed * .272) + (inputGreen *.534) + (inputBlue * .131)
			*/
		float r=colour[0];
		float g=colour[1];
		float b=colour[2];
		colour.x = r*.393f	+ g*.769f	+ b*.189f ;
		colour.y = r*.349f	+ g*.686f	+ b*.168f ;
		colour.z = r*.272f	+ g*.534f	+ b*.131f ;
	}
    FragmentColour = colour;
}

