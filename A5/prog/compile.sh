LIBS="-lglfw -lpthread -lm -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lGLU -ldl -lOpenGL "

g++ $LIBS ./main.cpp -Wall -g -Imiddleware/stb -Imiddleware/glad/include -Imiddleware -w
