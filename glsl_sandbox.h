//This file is a simplified version of the one at `extern/CxxSwizzle/sample/main.cpp`.

#ifndef GLSL_SANDBOX
#define GLSL_SANDBOX

#include "extern/CxxSwizzle/sample/use_scalar.h"

#include <swizzle/glsl/vector.h>

typedef swizzle::glsl::vector< float_type, 2 > vec2;
typedef swizzle::glsl::vector< float_type, 3 > vec3;
typedef swizzle::glsl::vector< float_type, 4 > vec4;
typedef swizzle::glsl::vector< uint_type, 2 > uvec2;
typedef swizzle::glsl::vector< uint_type, 3 > uvec3;
typedef swizzle::glsl::vector< uint_type, 4 > uvec4;

typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 2, 2> mat2;
typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 3, 3> mat3;
typedef swizzle::glsl::matrix< swizzle::glsl::vector, vec4::scalar_type, 4, 4> mat4;


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
    vec4 iMouse;
    vec2 iResolution(1024, 1024);

    //sampler2D diffuse("diffuse.png", sampler2D::Repeat);
    //sampler2D specular("specular.png", sampler2D::Repeat);
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

#endif
