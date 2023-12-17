#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Transform
{
public:
	Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
	~Transform() {};

	glm::vec3 GetPosition();
	glm::vec3 GetRotation();
	glm::vec3 GetScale();

	glm::mat4 GetModelMatrix();
	glm::mat4 GetTransposeInverseModelMatrix();

	void SetPosition(glm::vec3 newPosition);
	void SetRotation(float angleInDegrees, glm::vec3 axis);
	void SetScale(glm::vec3 newScale);

private:
	void CalculateModelMatrix();

	glm::vec3 m_CurrentPosition;
	glm::vec3 m_CurrentRotation;
	glm::vec3 m_CurrentScale;

	glm::mat4 m_ModelMatrix;
	glm::mat4 m_TransposeInverseModelMatrix;
};