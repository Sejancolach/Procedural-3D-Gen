#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;
out vec2 UV;
void main(){
	gl_Position = MVP * vec4(vertexPosition_modelspace,1);
	UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
}