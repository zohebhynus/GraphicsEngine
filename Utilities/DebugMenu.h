#pragma once
#include <vector>

#include "../GameEngineFeatures/GameObject.h"
#include "../Graphics/LightSystem.h"

struct ObjectOptions
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class DebugMenu
{
public:
	DebugMenu(std::vector<GameObject>& gameObjects, LightSystem& lightSystem, bool& editMode, GLFWwindow* window);
	~DebugMenu();

	void StartRender();
	void NewStartRender();
	void EndRender();
	void Shutdown();

	inline bool isEditMode() { return m_editMode; };

private:
	void RenderObjectList();
	void TransformationMenu();
	void SetObjectMenu(GameObject& obj);
	void SendObjectValues(GameObject& obj);

	void GeneralLightMenu();
	void SetAmbientLightMenu();
	void SendAmbientLightValues();

	void SetDirectionalLightMenu();
	void SendDirectionalLightValues();
	void DirectionalLightMenu();

	void SetPointLightMenu(PointLight& light);
	void SendPointLightValues();
	void PointLightListMenu();
	void RenderPointLightList();

	std::vector<GameObject>& m_gameObjects;
	LightSystem& m_LightSystem;
	bool& m_editMode;

	ObjectOptions objectOptions;

	GlobalAmbientLight ambientLightOptions;

	DirectionalLight directionalLightOptions;
	float dirAngle = 0;

	PointLight pointLightOptions;

	std::string m_SelectedPointLight;

	std::vector<const char*>m_TabOptions = { "Objects", "Lights", "ParticleSystem" };
	std::string m_SelectedTab = "Objects";
};