#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 fragNormal;
in vec3 fragNormalWS;
in vec3 fragPos;

layout (location = 8) uniform sampler2D TextureSampler1;
layout (location = 9) uniform sampler2D TextureSampler2;

float map(float val, float l1, float h1, float l2, float h2){
	return l2 + (val - l1) * (h2 -l2) / (h1-l1);
}

void main(){
	float theta = clamp(map(dot(fragNormal,vec3(0,1,0)),.8,.9,0,1),0,1);
	vec3 axisSign = sign(fragNormal);
	vec2 xUV = fragNormalWS.zy*.25f;
	vec2 yUV = fragNormalWS.xz*.25f;
	vec2 zUV = fragNormalWS.xy*.25f;

	vec3 xDiff = texture(TextureSampler1,xUV).rgb * theta
				+texture(TextureSampler2,xUV).rgb * (1-theta);
	vec3 yDiff = texture(TextureSampler1,yUV).rgb * theta
				+texture(TextureSampler2,yUV).rgb * (1-theta);
	vec3 zDiff = texture(TextureSampler1,zUV).rgb * theta
				+texture(TextureSampler2,zUV).rgb * (1-theta);
	
	vec3 blend = abs(fragNormal);
	blend = blend / (blend.x + blend.y + blend.z);

	gPosition = fragPos;
	gColor = xDiff * blend.x + yDiff * blend.y + zDiff * blend.z;

	gNormal = fragNormal;
}