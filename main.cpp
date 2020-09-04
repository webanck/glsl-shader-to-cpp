#include "glsl_sandbox.h"

using namespace glsl_sandbox;

int main(int argc, char ** argv)
{
	sampler2D image(sampler2D::WrapMode::Clamp, uvec2(iResolution));
	render(image, mainImage);
	
	write(image, "frame.png");
	
	return 0;
}
