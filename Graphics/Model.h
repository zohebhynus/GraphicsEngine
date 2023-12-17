#pragma once
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <fstream>
#include <json/json.hpp>
#include <assert.h>
#include <iostream>

#include "Mesh.h"

using json = nlohmann::json;

class Model
{
public:
	Model(std::string path)
	{

		std::ifstream f(path);
		m_Data = json::parse(f);

		if (m_Data["fileType"] != "Model")
		{
			std::cout << "JSON fileType is not [MODEL]" << std::endl;
			assert(false);
		}
		std::string filename = m_Data["filePath"];

		m_Directory = path.substr(0, path.find_last_of('/'));
		filename = m_Directory + '/' + filename;

		LoadModel(filename);
	}

	void Draw(Shader* shader);

private:
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Tex> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName, const aiScene* scene);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
	unsigned int TextureFromEmbeddedData(aiTexture* embeddedTexture);

	Tex LoadTextureJson(std::string path);

	std::vector<Mesh> m_Meshes;
	std::vector<Tex>m_LoadedTextures;
	std::string m_Directory;
	json m_Data;
};