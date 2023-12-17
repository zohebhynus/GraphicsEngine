#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;

uniform mat4 lightProjection;
uniform mat4 model;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


out vec2 v_TexCoord;
out vec3 v_FragPos;

void main()
{
	gl_Position = lightProjection * model * vec4(aPos, 1.0f);
}