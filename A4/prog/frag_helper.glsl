TRI_INTERCEPTION:
    #define a A[0][0]
    #define b A[1][0]
    #define c A[2][0]
    #define d A[0][1]
    #define e A[1][1]
    #define f A[2][1]
    #define g A[0][2]
    #define h A[1][2]
    #define i A[2][2]
    
    #define p ray[0]
    #define q ray[1]
    #define r ray[2]

    float t = (b*f*r-b*i*q-c*e*r+c*h*q+e*i*p-f*h*p) /(-a*e*i+a*f*h+b*d*i-b*f*g-c*d*h+c*e*g);
    float u =-(-a*f*r+a*i*q+c*d*r-c*g*q-d*i*p+f*g*p)/(-a*e*i+a*f*h+b*d*i-b*f*g-c*d*h+c*e*g);
    float v =-(a*e*r-a*h*q-b*d*r+b*g*q+d*h*p-e*g*p)/(-a*e*i+a*f*h+b*d*i-b*f*g-c*d*h+c*e*g);

    return(vec3(t,u,v));


CHECKER:

    if(colour[1] > 1 || colour[2] > 1 || 1-colour[1]-colour[2] <0 || colour [1] < 0 || colour[2] <0|| 1-colour[1]-colour[2] < 0){
        colour=vec4(0);
    }else{
        colour=vec4(1);
    }



