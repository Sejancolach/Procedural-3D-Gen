#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
in vec3 fragmentNormal;
in vec3 fragmentWorldSpaceNormal;
in vec4 ShadowCoord;
out vec3 color;

layout(location = 1)uniform sampler2D TextureSampler1;
layout(location = 2)uniform sampler2D TextureSampler2;
layout(location = 8) uniform sampler2DShadow shadowMap;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


void main(){
	float cosTheta = clamp(dot(fragmentNormal,vec3(0,1,0)),0,1);
	float bias = 0.005f*tan(acos(cosTheta));
	bias = clamp(bias,0,0.01);
	vec2 xUV = fragmentWorldSpaceNormal.zy;
	vec2 yUV = fragmentWorldSpaceNormal.xz;
	vec2 zUV = fragmentWorldSpaceNormal.xy;

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
	
	vec3 blend = abs(fragmentNormal);
	blend = blend / (blend.x + blend.y + blend.z);
	color = xDiff * blend.x + yDiff * blend.y + zDiff * blend.z;

	float visibility = 1.0f;
	for (int i=0;i<4;i++){
	  if ( texture( shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z - bias)/ShadowCoord.w)) < .5f){
		visibility-=0.15;
	  }
	}

	color *= (cosTheta+.5f)*.666f;
	color *= visibility;
}