#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 TexCoords;
void main() {
	TexCoords = (aPos.xy+vec2(1,1))/2.0;
	gl_Position = vec4(aPos, 1.0f);
}