#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;

struct Mat
{
	sampler2D albedo;
	sampler2D normal;
	sampler2D roughness;
	sampler2D ao;
};

uniform mat4 model;
uniform Mat material;

void main()
{
	gl_Position = model * vec4(aPos, 1.0f);
}