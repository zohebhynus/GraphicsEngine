#include "Mesh.h"
#include <iostream>
#include <format>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Tex> textures)
	: m_Vertices(vertices),
	m_Indices(indices),
	m_Textures(textures)
{
	setupMesh();
}

void Mesh::Draw(Shader* shader)
{
	bool isMetal = false;
	for (int i = 0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].type == "metallic")
		{
			isMetal = true;
			shader->SetUniform1i("material.isMetal", 1);
		}

		glActiveTexture(GL_TEXTURE0 + i);

		shader->SetUniform1i(std::format("material.{0}", m_Textures[i].type), i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].TextureId);
	}
	if (!isMetal)
	{
		shader->SetUniform1i("material.metallic", 0);
		shader->SetUniform1i("material.isMetal", 0);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoords));
	glBindVertexArray(0);
}
