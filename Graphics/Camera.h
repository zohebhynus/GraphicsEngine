#pragma once
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "../Input/MouseInput.h"

class Camera
{
public:
	Camera(glm::vec3 startingPosition, glm::vec3 cameraTarget, MouseInput* mouseInput);
	~Camera();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	void ProcessInput(GLFWwindow* window, double deltaTime);

	glm::vec3 GetPostion();

private:
	MouseInput* m_MouseInput;
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraTarget;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraRight;

	float m_Pitch;
	float m_Yaw;

	glm::mat4 m_ViewMatrix;
};