#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;

struct Light{
	vec3 Position;
	vec3 Color;
	float Linear;
	float Quadratic;
};
const int NR_LIGHTS = 255; //MAXING AMOUNT OF LIGHTS IS 255 !!!
uniform Light lights[NR_LIGHTS];

uniform vec3 viewPos;

void main(){
	vec3 FragPos = texture(gPosition, TexCoords).xyz;
	vec3 Normal  = texture(gNormal, TexCoords).xyz;
	vec3 Diffuse = texture(gColor, TexCoords).rgb;

	vec3 lightning = Diffuse * .2;
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
	//FragColor = vec4(Diffuse,1);
}