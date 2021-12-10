#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 diffuseColor;

void main(){
	gPosition = fragPos;
	gNormal = fragNormal;
	gColor = diffuseColor;
}