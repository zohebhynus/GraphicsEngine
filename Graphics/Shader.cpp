#include "Renderer.h"
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

std::string GetFileContent(const char* filePath)
{
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss;

	while (std::getline(stream, line))
	{
		ss << line << '\n';
	}
	return ss.str();
}

Shader::Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
	std::string vertexCode = GetFileContent(vertexFile);
	std::string fragmentCode = GetFileContent(fragmentFile);
	std::string geometryCode;
	if (geometryFile != nullptr)
		geometryCode = GetFileContent(geometryFile);


	// Print Vertex and Fragment Shader
	std::cout << "VERTEX" << std::endl << vertexCode << std::endl;
	std::cout << "FRAGMENT" << std::endl << fragmentCode << std::endl;
	if (geometryFile != nullptr)
		std::cout << "GEOMETRY" << std::endl << geometryCode << std::endl;


	GLuint vs = Compile(GL_VERTEX_SHADER, vertexCode);
	GLuint fs = Compile(GL_FRAGMENT_SHADER, fragmentCode);
	GLuint gs = 0;
	if (geometryFile != nullptr)
	{
		gs = Compile(GL_GEOMETRY_SHADER, geometryCode);
	}


	// combine and create shader program
	m_RendererId = glCreateProgram();
	GLCall(glAttachShader(m_RendererId, vs));
	GLCall(glAttachShader(m_RendererId, fs));
	if (geometryFile != nullptr)
		(glAttachShader(m_RendererId, gs));
	GLCall(glLinkProgram(m_RendererId));

	//Printing Status of shader program and error
	GLint program_linked;

	GLCall(glGetProgramiv(m_RendererId, GL_LINK_STATUS, &program_linked));
	std::cout << "Program link status: " << (program_linked == GL_TRUE ? "SUCCESS" : "FAIL") << std::endl;
	if (program_linked == GL_FALSE)
	{
		char message[512];
		GLCall(glGetProgramInfoLog(m_RendererId, 512, NULL, message));
		std::cout << "Failed to link program" << std::endl;
		std::cout << message << std::endl;
	}

	GLCall(glUseProgram(m_RendererId));

	// delete vertex and fragment shader
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	if (geometryFile != nullptr)
	{
		(glDeleteShader(gs));
	}
}

Shader::Shader(const char* computeFile)
{

	std::string computeCode = GetFileContent(computeFile);
	std::cout << "COMPUTE SHADER ONLY" << std::endl << computeCode << std::endl;


	GLuint cs = Compile(GL_COMPUTE_SHADER, computeCode);



	// combine and create shader program

	m_RendererId = glCreateProgram();
	GLCall(glAttachShader(m_RendererId, cs));
	GLCall(glLinkProgram(m_RendererId));

	//Printing Status of shader program and error
	GLint program_linked;

	GLCall(glGetProgramiv(m_RendererId, GL_LINK_STATUS, &program_linked));
	std::cout << "Program link status: " << (program_linked == GL_TRUE ? "SUCCESS" : "FAIL") << std::endl;
	if (program_linked == GL_FALSE)
	{
		char message[512];
		GLCall(glGetProgramInfoLog(m_RendererId, 512, NULL, message));
		std::cout << "Failed to link program" << std::endl;
		std::cout << message << std::endl;
	}

	GLCall(glUseProgram(m_RendererId));

	// delete vertex and fragment shader
	GLCall(glDeleteShader(cs));
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererId));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererId));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(GetUniformLocation(name), f0, f1, f2, f3));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniformMatrix4fv(const std::string& name, glm::mat4 mat)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat)));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1b(const std::string& name, bool value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

int Shader::GetUniformLocation(const std::string name)
{
	if (m_UniformLocations.find(name) != m_UniformLocations.end())
	{
		return m_UniformLocations[name];
	}

	GLCall(GLuint location = glGetUniformLocation(m_RendererId, name.c_str()));

	//Error printing
	if (location == -1)
	{
		std::cout << "No active uniform variable with name " << name << " found" << std::endl;
	}

	m_UniformLocations[name] = location;
	return location;
}

GLuint Shader::Compile(GLuint type, const std::string& source)
{
	// vertex shader compile
	GLCall(GLuint shaderId = glCreateShader(type));
	const char* str = source.c_str();
	GLCall(glShaderSource(shaderId, 1, &str, NULL));
	GLCall(glCompileShader(shaderId));

	//Printing Status of shader and error
	int result;
	GLCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));


	const char* stringType;
	if (type == GL_VERTEX_SHADER)
		stringType = "vertex";
	else if (type == GL_FRAGMENT_SHADER)
		stringType = "fragment";
	else if (type == GL_GEOMETRY_SHADER)
		stringType = "geometry";
	else
		stringType = "compute";

	std::cout << stringType << " shader compile status: " << (result == GL_TRUE ? "SUCCESS" : "FAIL") << std::endl;
	if (result == GL_FALSE)
	{
		int length;
		char message[512];
		GLCall(glGetShaderInfoLog(shaderId, 512, NULL, message));
		std::cout
			<< "Failed to compile "
			<< stringType
			<< "shader"
			<< std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(shaderId));
		return 0;
	}

	return shaderId;
}
