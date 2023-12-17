#pragma once
#include<vector>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"


struct GlobalAmbientLight
{
	glm::vec3 LightColor;
	float ColorIntensity;

	bool IsEnabled = true;
};

struct DirectionalLight
{
	glm::vec3 Direction;

	glm::vec3 LightColor;
	float ColorIntensity;
	float SpecularIntensity;

	bool IsEnabled = true;

	inline DirectionalLight operator=(DirectionalLight rhs)
	{
		this->Direction.x = rhs.Direction.x;
		this->Direction.y = rhs.Direction.y;
		this->Direction.z = rhs.Direction.z;

		this->LightColor.x = rhs.LightColor.x;
		this->LightColor.y = rhs.LightColor.y;
		this->LightColor.z = rhs.LightColor.z;

		this->ColorIntensity = rhs.ColorIntensity;
		this->SpecularIntensity = rhs.SpecularIntensity;

		this->IsEnabled = rhs.IsEnabled;

		return rhs;
	}
};

struct ptLight
{
	glm::vec4 Position;

	glm::vec4 LightColor;

	glm::vec4 RadiusIntensity;
};

struct VisibleIndex {
	int index;
};


struct PointLight
{
	std::string Name;

	glm::vec3 Position;

	glm::vec3 LightColor;

	float radius;
	float edgeIntensity;

	inline PointLight operator=(PointLight rhs)
	{

		this->Position.x = rhs.Position.x;
		this->Position.y = rhs.Position.y;
		this->Position.z = rhs.Position.z;

		this->LightColor.x = rhs.LightColor.x;
		this->LightColor.y = rhs.LightColor.y;
		this->LightColor.z = rhs.LightColor.z;

		this->radius = rhs.radius;
		this->edgeIntensity = rhs.edgeIntensity;

		return rhs;
	}
};

class LightSystem
{
public:
	LightSystem(Shader* lightShader, Shader* m_LightObjectShader, Model* m_LightObjectModel);
	~LightSystem() {};

	void SetLightData();

	GlobalAmbientLight GetAmbientLight();
	void SetAmbientLight(GlobalAmbientLight ambientLight);

	DirectionalLight GetDirectionalLight();
	void SetDirectionalLight(DirectionalLight dirLight);

	std::vector<PointLight> GetPointLightList();
	void SetPointLight(std::string name, PointLight pointLight);

	void DrawLightObjects(Camera& camera);
	int NUMBER_OF_LIGHTS = 10;

private:
	GlobalAmbientLight m_AmbientLight;
	DirectionalLight m_DirectionalLight;
	std::vector<PointLight> m_PointLights;

	Shader* m_LightShader;
	Shader* m_LightObjectShader;

	Model* m_LightObjectModel;
};