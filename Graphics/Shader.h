#pragma once
#include <glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <string>
#include <unordered_map>

std::string GetFileContent(const char* filePath);

class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
	Shader(const char* computeFile);
	~Shader();

	void Bind() const;
	void UnBind() const;

	//Uniform functions
	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
	void SetUniform1f(const std::string& name, float value);

	void SetUniformMatrix4fv(const std::string& name, glm::mat4 mat);

	void SetUniform1i(const std::string& name, int value);

	void SetUniform1b(const std::string& name, bool value);

private:
	GLuint m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocations;

	int GetUniformLocation(const std::string name);
	GLuint Compile(GLuint type, const std::string& source);
};