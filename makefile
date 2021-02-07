shader: glsl_sandbox.h shader.glsl main.cpp
	g++ main.cpp -fopenmp -I ./extern/CxxSwizzle/include -std=c++11 -fno-operator-names -O3 -lSDL2 -I/usr/include/SDL2 -D_REENTRANT -lSDL2_image -o shader
