#pragma once
#include<string>
#include<vector>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader.h"



struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 Texcoords;
};

struct Tex
{
	unsigned int TextureId;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Tex>textures);
	~Mesh() {};

	void Draw(Shader* shader);

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Tex> m_Textures;

	unsigned int VAO, VBO, EBO;
	//VertexBuffer m_VBO;
	//IndexBuffer m_IBO;
	//VertexArray m_VAO;
	//VertexBufferLayout m_BufferLayout;

	void setupMesh();
};