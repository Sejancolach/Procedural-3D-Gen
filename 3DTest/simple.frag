#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentWorldSpaceNormal;
out vec3 color;

uniform sampler2D TextureSampler1;
uniform sampler2D TextureSampler2;

void main(){
	float cosTheta = clamp(dot(fragmentNormal,vec3(0,1,0)),0,1);
	
	vec2 xUV = fragmentWorldSpaceNormal.zy;
	vec2 yUV = fragmentWorldSpaceNormal.xz;
	vec2 zUV = fragmentWorldSpaceNormal.xy;

	vec3 xDiff;
	vec3 yDiff;
	vec3 zDiff;
	if (cosTheta > .9f){
		xDiff = texture(TextureSampler1, xUV).rgb;
		yDiff = texture(TextureSampler1, yUV).rgb;
		zDiff = texture(TextureSampler1, zUV).rgb;
	}	
	else {
		xDiff = texture(TextureSampler2, xUV).rgb;
		yDiff = texture(TextureSampler2, yUV).rgb;
		zDiff = texture(TextureSampler2, zUV).rgb;
	}
	
	vec3 blend = abs(fragmentNormal);
	blend = blend / (blend.x + blend.y + blend.z);
	color = xDiff * blend.x + yDiff * blend.y + zDiff * blend.z;


	color *= cosTheta;
	//color = fragmentNormal;
}