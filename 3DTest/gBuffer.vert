#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform vec3 worldPosition;
uniform mat4 MVP;
uniform mat4 LightBiasMVP;

out vec3 fragNormal;
out vec3 fragNormalWS;
out vec3 fragPos;
out vec4 ShadowCoord;

void main(){
	vec3 wPos = worldPosition + aPos;
	fragPos = wPos;
	fragNormal = aNormal;
	fragNormalWS = fragNormal + wPos;
	ShadowCoord = LightBiasMVP * vec4(aPos,1);

	gl_Position = MVP * vec4(aPos,1);
//	fragNormal = aNormal;
//	fragPos = aPos * worldPosition;
//	fragNormalWS = fragNormal + (aPos + worldPosition);
//	gl_Position = MVP * vec4(aPos,1);
}