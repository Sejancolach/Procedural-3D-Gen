#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

uniform vec3 worldPosition;
uniform mat4 MVP;
out vec3 fragmentColor;
out vec3 fragmentNormal;
out vec3 fragmentWorldSpaceNormal;

void main(){
	gl_Position = MVP * vec4(vertexPosition_modelspace,1);
	fragmentNormal = vertexNormal;
	fragmentWorldSpaceNormal = fragmentNormal + (vertexPosition_modelspace + worldPosition);
}