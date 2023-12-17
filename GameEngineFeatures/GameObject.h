#pragma once
#include <string>
#include <iostream>

#include "../Graphics/Shader.h"
#include "../Graphics/Model.h"
#include "Transform.h"
#include "../Graphics/Camera.h"



class GameObject
{
public:
	GameObject(const char* name = "Object", Model* model = nullptr, Shader* shader = nullptr);
	~GameObject() {};

	std::string GetName();
	Transform& GetTransform();

	void SetName(std::string newName);

	void Draw(Camera& camera);

	void DepthDraw(Shader* shadowMapShader);

private:
	std::string m_Name;
	Transform m_Transform;

	Model* m_Model;
	Shader* m_Shader;
};