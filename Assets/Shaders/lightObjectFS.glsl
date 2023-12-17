#version 430 core
uniform vec4 lightColor;

in vec2 v_TexCoord;
in vec3 v_FragPos;

out vec4 FragColor;

void main()
{
	FragColor = lightColor;
}