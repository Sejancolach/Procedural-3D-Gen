#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
in vec3 fragmentNormal;
in vec3 fragmentWorldSpaceNormal;
out vec3 color;

void main(){
	float cosTheta = clamp(dot(fragmentNormal,vec3(0,1,0)),0,1) * .5f + .25f;

	color = abs(fragmentWorldSpaceNormal) * cosTheta;

}