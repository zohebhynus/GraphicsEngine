#include "Model.h"
#include <stb/stb_image.h>
#include <iostream>
void Model::Draw(Shader* shader)
{
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Draw(shader);
	}
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Tex> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.Texcoords.x = mesh->mTextureCoords[0][i].x;
			vertex.Texcoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.Texcoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (m_Data["haveMaterial"] == "true")
	{
		// albedo map
		std::string path = m_Data["material"]["albedo"];
		path = m_Directory + '/' + path;

		Tex text = LoadTextureJson(path);
		text.type = "albedo";
		textures.push_back(text);

		// normal map
		path = m_Data["material"]["normal"];
		path = m_Directory + '/' + path;

		text = LoadTextureJson(path);
		text.type = "normal";
		textures.push_back(text);

		// metallic map
		path = m_Data["material"]["metallic"];
		if (path != "none")
		{
			path = m_Directory + '/' + path;

			text = LoadTextureJson(path);
			text.type = "metallic";
			textures.push_back(text);
		}

		// roughness map
		path = m_Data["material"]["roughness"];
		path = m_Directory + '/' + path;

		text = LoadTextureJson(path);
		text.type = "roughness";
		textures.push_back(text);

		// ao map
		path = m_Data["material"]["ao"];
		path = m_Directory + '/' + path;

		text = LoadTextureJson(path);
		text.type = "ao";
		textures.push_back(text);
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Tex> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<Tex> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		// Check if the texture is embedded
		if (str.C_Str()[0] == '*')
		{
			int embeddedTextureIndex = atoi(&str.C_Str()[1]);
			if (embeddedTextureIndex >= 0 && embeddedTextureIndex < scene->mNumTextures)
			{
				aiTexture* embeddedTexture = scene->mTextures[embeddedTextureIndex];
				Tex texture;
				texture.TextureId = TextureFromEmbeddedData(embeddedTexture);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
			}
		}
		else
		{
			bool skip = false;
			for (int j = 0; j < m_LoadedTextures.size(); j++)
			{
				if (std::strcmp(m_LoadedTextures[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(m_LoadedTextures[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				Tex texture;
				texture.TextureId = TextureFromFile(str.C_Str(), m_Directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				m_LoadedTextures.push_back(texture);
			}
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{

		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int Model::TextureFromEmbeddedData(aiTexture* embeddedTexture)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, embeddedTexture->mWidth, embeddedTexture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, embeddedTexture->pcData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

Tex Model::LoadTextureJson(std::string path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{

		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	Tex text;
	text.TextureId = textureID;
	text.path = path;

	return text;
}