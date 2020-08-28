//adapted from gkit image_io.cpp
#include <vector>
#include <SDL2/SDL_image.h>
int write_image(const char* filename)
{
    const uint width = glsl_sandbox::iResolution.x;
    const uint height = glsl_sandbox::iResolution.y;

    if(std::string(filename).rfind(".png") == std::string::npos && std::string(filename).rfind(".bmp") == std::string::npos )
    {
        printf("[error] writing color image '%s'... not a .png / .bmp image.\n", filename);
        return -1;
    }

    // flip de l'image : Y inverse entre GL et BMP
    std::vector<Uint8> flip(width * height * 4);

    int p= 0;
    for(int y= 0; y < height; y++)
    for(int x= 0; x < width; x++)
    {
        vec4 color;
        glsl_sandbox::mainImage(color, vec2(x, height - y -1));

        Uint8 r= (Uint8) std::min(std::floor(color.x * 255.f), 255.f);
        Uint8 g= (Uint8) std::min(std::floor(color.y * 255.f), 255.f);
        Uint8 b= (Uint8) std::min(std::floor(color.z * 255.f), 255.f);
        Uint8 a= (Uint8) std::min(std::floor(color.w * 255.f), 255.f);

        flip[p]= r;
        flip[p +1]= g;
        flip[p +2]= b;
        flip[p +3]= a;
        p= p + 4;
    }

    SDL_Surface *surface= SDL_CreateRGBSurfaceFrom((void *) &flip.front(), width, height, 32, width * 4,
#if 0
        0xFF000000,
        0x00FF0000,
        0x0000FF00,
        0x000000FF
#else
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
#endif
    );

    int code= -1;
    if(std::string(filename).rfind(".png") != std::string::npos)
        code= IMG_SavePNG(surface, filename);
    else if(std::string(filename).rfind(".bmp") != std::string::npos)
        code= SDL_SaveBMP(surface, filename);

    SDL_FreeSurface(surface);
    if(code < 0)
        printf("[error] writing color image '%s'...\n%s\n", filename, SDL_GetError());
    return code;
}
