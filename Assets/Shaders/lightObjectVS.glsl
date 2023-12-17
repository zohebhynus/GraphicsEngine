#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;
out vec3 v_FragPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	v_FragPos = vec3(model *  vec4(aPos, 1.0f));
	v_TexCoord = vec2(texCoord.x, texCoord.y);
}