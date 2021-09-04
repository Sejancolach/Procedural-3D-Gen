#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) out vec3 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 fragNormal;
in vec3 fragNormalWS;
in vec3 fragPos;

layout (location = 8) uniform sampler2D TextureSampler1;
layout (location = 9) uniform sampler2D TextureSampler2;

void main(){
	float cosTheta = clamp(dot(fragNormal,vec3(0,1,0)),0,1);
	float bias = 0.005f*tan(acos(cosTheta));
	vec2 xUV = fragNormalWS.zy;
	vec2 yUV = fragNormalWS.xz;
	vec2 zUV = fragNormalWS.xy;

	vec3 xDiff;
	vec3 yDiff;
	vec3 zDiff;
	if (cosTheta >= .9f){
		xDiff = texture(TextureSampler1, xUV).rgb;
		yDiff = texture(TextureSampler1, yUV).rgb;
		zDiff = texture(TextureSampler1, zUV).rgb;
	}	
	else if (cosTheta >= .8f){
		float t = (cosTheta - 0.8f) * 10;		
		xDiff = texture(TextureSampler1, xUV).rgb * t;
		yDiff = texture(TextureSampler1, yUV).rgb * t;
		zDiff = texture(TextureSampler1, zUV).rgb * t;
		t = 1 - t;
		xDiff += texture(TextureSampler2, xUV).rgb * t;
		yDiff += texture(TextureSampler2, yUV).rgb * t;
		zDiff += texture(TextureSampler2, zUV).rgb * t;
	}
	else{
		xDiff = texture(TextureSampler2, xUV).rgb;
		yDiff = texture(TextureSampler2, yUV).rgb;
		zDiff = texture(TextureSampler2, zUV).rgb;
	}
	
	vec3 blend = abs(fragNormal);
	blend = blend / (blend.x + blend.y + blend.z);

	gPositionDepth = fragPos;
	gColor = xDiff * blend.x + yDiff * blend.y + zDiff * blend.z;
	gNormal = fragNormal;
}