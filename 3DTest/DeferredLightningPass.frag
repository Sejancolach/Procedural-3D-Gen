#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
out vec4 FragColor;

in vec2 TexCoords;

layout(location = 0) uniform sampler2D gPosition;
layout(location = 1) uniform sampler2D gNormal;
layout(location = 2) uniform sampler2D gColor;
layout(location = 7) uniform sampler2DShadow ShadowMap;

struct Light{
	vec3 Position;
	vec3 Color;
	float Linear;
	float Quadratic;
};
const int NR_LIGHTS = 127;
uniform Light lights[NR_LIGHTS];
//change from array to storing it in a texture

uniform vec3 viewPos;

uniform mat4 ShadowMapMVP;
uniform vec3 sunViewDir;
uniform mat4 MVP;

//Shadow mapping
const int POISSONDISKSIZE = 8;
const vec2 poissonDisk[POISSONDISKSIZE] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 ),
  vec2( -0.44201624, -0.59906216 ),
  vec2( 0.14558609, -0.36890725 ),
  vec2( -0.34184101, -0.092938870 ),
  vec2( 0.64495938, 0.79387760 )
);

const float POISSON_DISK_BIAS = 0.0000152587890625;
const float SHADOW_BIAS = 0.000125f;

void main(){
	vec3 FragPos = texture(gPosition, TexCoords).xyz;
	vec3 Normal  = texture(gNormal, TexCoords).xyz;
	vec3 Diffuse = texture(gColor, TexCoords).rgb;

	//Shadow calculation
	float cosTheta = clamp(dot(Normal,sunViewDir),0,1);
	float shadowBias = SHADOW_BIAS * tan(acos(cosTheta));
	shadowBias = clamp(shadowBias,0, SHADOW_BIAS * 4);
	vec4 ShadowCoord = ShadowMapMVP * vec4(FragPos,1);

	float visibility = 1f;
	if (ShadowCoord.x < 1 && ShadowCoord.x > 0 && ShadowCoord.y < 1 && ShadowCoord.y > 0){
		for (int i = 0; i < POISSONDISKSIZE; i++){
		  if ( texture( ShadowMap, vec3(ShadowCoord.xy + poissonDisk[i] * POISSON_DISK_BIAS , (ShadowCoord.z - shadowBias)/ShadowCoord.w)) < .75f){
			const float sbt = .7f / POISSONDISKSIZE;
			visibility -=sbt;
		  }
		}
	}

	vec3 lightning = Diffuse * visibility;

	vec3 viewDir = normalize(viewPos - FragPos);
	for (int i = 0; i < NR_LIGHTS; i++){
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
	
		float dist = length(lights[i].Position - FragPos);
		float attentuation = 1.0 / (1.0 + lights[i].Linear * dist + lights[i].Quadratic * dist * dist);
		diffuse *= attentuation;
		lightning += diffuse;
	}
	FragColor = vec4(lightning, 1.0);
}
