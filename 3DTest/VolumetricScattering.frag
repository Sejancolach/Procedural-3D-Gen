#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 TexCoords;

layout(location = 0) uniform sampler2D gPosition;
layout(location = 1) uniform sampler2D gNormal;
layout(location = 2) uniform sampler2D gColor;
layout(location = 7) uniform sampler2DShadow ShadowMap;

uniform vec3 CameraPosition;
uniform vec3 sunDir;
uniform mat4 ShadowMapMVP;
out vec4 color;

const int NSTEPS = 128;

void main(){
    vec3 worldPos = texture(gPosition,TexCoords).xyz;
    vec3 startPos = CameraPosition;
    vec3 ray = worldPos - startPos;
    float rayLen = length(ray);
    vec3 rayDir = ray / rayLen;
    float sLen = rayLen / NSTEPS;
    vec3 rayStep = ray * sLen;
    vec3 cPos = startPos;
    vec3 vs = vec3(0,0,0);
    for (int i = 0; i < NSTEPS; i++){
        vec4 ShadowCoord = vec4(cPos, 1.0f) * ShadowMapMVP;
        ShadowCoord /= ShadowCoord.w;
        if( texture(ShadowMap,vec3(ShadowCoord.xy, (ShadowCoord.z )/ShadowCoord.w)) < .5f){
            vs += (dot(rayDir, sunDir)) * vec3(1,1,1);
        }
        cPos += rayStep;
    }
    vs /= NSTEPS;
    color = texture(gColor, TexCoords) + vec4(vs,1f);
}