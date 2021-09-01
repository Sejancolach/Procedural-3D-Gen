#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
in vec2 UV;

out vec4 color;
layout(location = 8) uniform sampler2D renderedTexture;

void main(){
    color = texture( renderedTexture, UV);
    //color = vec4(1,1,1,1);
}