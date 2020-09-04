//This file is a simplified version of the one at `extern/CxxSwizzle/sample/main.cpp`.

#ifndef GLSL_SANDBOX
#define GLSL_SANDBOX

#include "extern/CxxSwizzle/sample/use_scalar.h"

#include <swizzle/glsl/vector.h>
#include <swizzle/glsl/texture_functions.h>

#include <vector>

typedef swizzle::glsl::vector< float_type, 2 > vec2;
typedef swizzle::glsl::vector< float_type, 3 > vec3;
typedef swizzle::glsl::vector< float_type, 4 > vec4;
typedef swizzle::glsl::vector< uint_type, 2 > ivec2;
typedef swizzle::glsl::vector< uint_type, 3 > ivec3;
typedef swizzle::glsl::vector< uint_type, 4 > ivec4;
typedef swizzle::glsl::vector< uint_type, 2 > uvec2;
typedef swizzle::glsl::vector< uint_type, 3 > uvec3;
typedef swizzle::glsl::vector< uint_type, 4 > uvec4;

typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 2, 2> mat2;
typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 3, 3> mat3;
typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 4, 4> mat4;



class sampler2D : public swizzle::glsl::texture_functions::tag
{
public:
    enum WrapMode
    {
        Clamp,
        Repeat,
        MirrorRepeat
    };

    typedef const vec2& tex_coord_type;

	sampler2D(const WrapMode wrapMode, const uvec2& rResolution) :
		m_wrapMode(wrapMode),
		m_resolution(rResolution),
		m_image(rResolution.x * rResolution.y)
	{
	}
    vec4 sample(const vec2& coord) const;
	
private:
    const WrapMode m_wrapMode;
    const uvec2 m_resolution;
    std::vector<vec4> m_image;
    uint pixelId(const uvec2& p) const
    {
		return p.x + p.y*m_resolution.x;
		//return p.x*m_resolution.y + p.y;
	}
	uvec2 pixelCoord(const uint id)
	{
		return uvec2(
			id%m_resolution.x,
			id/m_resolution.x
		);
	}

    // do not allow copies to be made
    sampler2D(const sampler2D&) = delete;
    sampler2D& operator=(const sampler2D&) = delete;
    
	friend void render(sampler2D&, void (*mainImage)(vec4&, const vec2&));
	friend void write(const sampler2D&, const char*);
};




// this where the magic happens...
namespace glsl_sandbox
{
    // a nested namespace used when redefining 'inout' and 'out' keywords
    namespace ref
    {
#ifdef CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED
        typedef swizzle::detail::vector_inout_wrapper<vec2> vec2;
        typedef swizzle::detail::vector_inout_wrapper<vec3> vec3;
        typedef swizzle::detail::vector_inout_wrapper<vec4> vec4;
#else
        typedef vec2& vec2;
        typedef vec3& vec3;
        typedef vec4& vec4;
#endif
        typedef ::float_type& float_type;
        typedef ::bool_type& bool_type;
    }

    namespace in
    {
        typedef const ::vec2& vec2;
        typedef const ::vec3& vec3;
        typedef const ::vec4& vec4;
        typedef const ::float_type float_type;
        typedef const ::bool_type bool_type;
    }

    #include <swizzle/glsl/vector_functions.h>

    // constants some shaders from shader toy are using
    float_type iTime = 0.;
    float_type iFrame = 0.;
    vec4 iMouse;
    vec2 iResolution(1024, 1024);

    //sampler2D diffuse("diffuse.png", sampler2D::Repeat);
    //sampler2D specular("specular.png", sampler2D::Repeat);
    sampler2D iChannel0(sampler2D::Clamp, uvec2(iResolution));
    //
    //struct fragment_shader
    //{
    //    vec2 gl_FragCoord;
    //    vec4 gl_FragColor;
    //    void operator()(void);
    //};

    // change meaning of glsl keywords to match sandbox
    #define uniform extern
    #define in in::
    #define out ref::
    #define inout ref::
    //#define main fragment_shader::operator()
    #define float float_type   
    #define bool bool_type
    
    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning
    
    #include "shader.glsl"
    //#include "extern/shader-null-scattering-framework/common.glsl"
    //namespace bufferA
    //{
	//	#include "extern/shader-null-scattering-framework/bufferA.glsl"
	//}
    //#include "extern/shader-null-scattering-framework/image.glsl"
    //#include "extern/shader-triangle-cut/image.glsl"
    
    // be a dear a clean up
    #pragma warning(pop)
    #undef bool
    #undef float
    #undef main
    #undef in
    #undef out
    #undef inout
    #undef uniform
}



vec4 sampler2D::sample(const vec2& coord) const
{
	using namespace glsl_sandbox;
	vec2 uv;
	switch(m_wrapMode)
	{
		case Repeat:
			uv = mod(coord, 1);
			break;
		case MirrorRepeat:
			uv = abs(mod(coord - 1, 2) - 1);
			break;
		case Clamp:
		default:
			uv = clamp(coord, 0, 1);
			break;
	}
	
	const uvec2 p = uvec2(uv * vec2(m_resolution));
	const uint id = pixelId(p);
	return m_image.at(id);
}

void render(sampler2D& rSampler, void (*mainImage)(vec4&, const vec2&))
{
	const uint pixelsNumber = rSampler.m_resolution.x * rSampler.m_resolution.y;
	
	#pragma omp parallel for
	for(uint id = 0u; id < pixelsNumber; id++)
	{
		vec4& rColor = rSampler.m_image.at(id);
		mainImage(rColor, vec2(rSampler.pixelCoord(id)));
	}
}

//adapted from gkit image_io.cpp
#include <vector>
#include <SDL2/SDL_image.h>
void write(const sampler2D& rSampler, const char * filename)
{
    const uint width = rSampler.m_resolution.x;
    const uint height = rSampler.m_resolution.y;

    if(std::string(filename).rfind(".png") == std::string::npos && std::string(filename).rfind(".bmp") == std::string::npos )
    {
        printf("[error] writing color image '%s'... not a .png / .bmp image.\n", filename);
        //return -1;
        return;
    }

    // flip de l'image : Y inverse entre GL et BMP
    std::vector<Uint8> flip(width * height * 4);

    int p = 0;
    for(int y= 0; y < height; y++)
    for(int x= 0; x < width; x++)
    {
        const vec4 color = rSampler.sample(vec2(x, height - y -1));

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
    //return code;
}



#endif
