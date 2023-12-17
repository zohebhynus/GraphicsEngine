#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 inverseTransposeModel;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightProjection;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 fragPosLight;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	v_FragPos = vec3(model *  vec4(aPos, 1.0f));
	v_TexCoord = vec2(texCoord.x, texCoord.y);
	v_Normal = mat3(inverseTransposeModel) * aNormal;
	fragPosLight = lightProjection * (model * vec4(aPos, 1.0f));
}