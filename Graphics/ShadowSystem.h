#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include "Shader.h"
#include "LightSystem.h"
#include "../GameEngineFeatures/GameObject.h"

class ShadowSystem
{
public:
	ShadowSystem(Shader* dirLightShadowMapShader, Shader* pointLightShadowCubeMapShader, LightSystem* lightSystem);
	~ShadowSystem() {};

	void GenerateDirLightShadowMap(std::vector<GameObject>& objectList);
	void RenderDirLightDebugQuad();

	void SetShaderValuesInLightShader(Shader* lightShader);

	void GeneratePointLightShadowCubeMap(std::vector<GameObject>& objectList);

private:

	void SetUpDirectionalLightShadow();
	void SetUpPointLightShadow();
	//Dir Light
	unsigned int m_DirLightShadowMapFBO;

	unsigned int m_DirLightShadowMapTexture;

	unsigned int m_DirLightShadowMapWidth;
	unsigned int m_DirLightShadowMapHeight;

	glm::mat4 m_LightProjection;

	Shader* m_DirLightShadowMapShader;
	Shader* m_DirLightDebugQuadShader;

	//Point Light
	float far_value = 100.0f;

	std::vector<unsigned int> m_PointLightShadowMapFBO;

	std::vector<unsigned int> m_PointLightShadowCubeMapTexture;

	std::unordered_map<std::string, std::vector<glm::mat4>> m_MapOfLightSpaceTransformations;

	Shader* m_PointLightShadowCubeMapShader;

	LightSystem* m_LightSystem;
};