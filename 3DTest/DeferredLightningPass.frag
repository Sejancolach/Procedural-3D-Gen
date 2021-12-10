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
uniform mat4 viewProjection;
uniform mat4 viewView;

uniform mat4 ShadowMapMVP;
uniform vec3 sunViewDir;
uniform vec3 sunPos;
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

const vec4 ditherPattern[4] = vec4[](
vec4( 0.0f, 0.5f, 0.125f, 0.625f),
vec4( 0.75f, 0.22f, 0.875f, 0.375f),
vec4( 0.1875f, 0.6875f, 0.0625f, 0.5625),
vec4( 0.9375f, 0.4375f, 0.8125f, 0.3125));

const float SHADOW_BIAS = 0.000125f;



const float G_SCATTERING = 0.00001f;
const float PI = 3.14159265359;

float ComputeScattering(float lightDotView) {
	float result = 1.0f - G_SCATTERING * G_SCATTERING;
	result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));
	return result;
}



void main(){
	vec3 FragPos = texture(gPosition, TexCoords).xyz;
	vec3 Normal  = texture(gNormal, TexCoords).xyz;
	vec3 Diffuse = texture(gColor, TexCoords).rgb;

	vec2 rTexCoords = TexCoords * 2 - vec2(1,1);
	ivec2 screenSpacePosition = ivec2(rTexCoords.x * 1440, rTexCoords.y * 810);

	//Shadow calculation
	float cosTheta = clamp(dot(Normal,sunViewDir),0,1);
	float shadowBias = SHADOW_BIAS * tan(acos(cosTheta));
	shadowBias = clamp(shadowBias,0, SHADOW_BIAS * 4);
	vec4 ShadowCoord = ShadowMapMVP * vec4(FragPos,1);

	float visibility = 1f;
	if (ShadowCoord.x < 1 && ShadowCoord.x > 0 && ShadowCoord.y < 1 && ShadowCoord.y > 0){
		for (int i = 0; i < POISSONDISKSIZE; i++){
		  if ( texture( ShadowMap, vec3(ShadowCoord.xy + poissonDisk[i] * POISSON_DISK_BIAS , (ShadowCoord.z - shadowBias)/ShadowCoord.w)) < .5f){
			const float sbt = .8f / POISSONDISKSIZE;
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

	const int fogSteps = 256;
	const float fogDensity = .75f;
	const float Weight = .025f;
	const float Decay = .995f;

	vec4 SSSunPos = (viewProjection * viewView) * vec4(sunPos,1);
	SSSunPos.x /= SSSunPos.w;
	SSSunPos.y /= SSSunPos.w;
	SSSunPos.z /= SSSunPos.w;
	SSSunPos.w = 1;

	vec3 sPos = viewPos;
	vec3 rayVector = FragPos.xyz - sPos;
	float rayLength = length(rayVector);
	vec3 rayDir = rayVector / rayLength;
	float stepLength = rayLength / fogSteps;
	vec3 rstep = rayDir * stepLength;
	vec3 cPos = sPos;
	vec3 accFog = vec3(-1,-1,-1);
	float illuminationDecay = 1.0f;
	float ditherValue = ditherPattern[screenSpacePosition.x % 4][screenSpacePosition.y % 4];

	sPos += ditherValue;
	for (int i = 0; i < fogSteps; i++){
		vec4 SCS = ShadowMapMVP  * vec4(cPos,1);
		SCS /= SCS.w;
		float smv = texture(ShadowMap, vec3(SCS.xyz));
		if (smv > SCS.z){
			accFog += ComputeScattering(dot(rayDir,sunViewDir)) * vec3(1,1,1) * illuminationDecay;
		}
		cPos += rstep;
		illuminationDecay *= Decay;
	}
	accFog /= float(fogSteps)*(1 - fogDensity);

	FragColor = vec4(lightning, 1.0) + vec4(accFog,0);
}
