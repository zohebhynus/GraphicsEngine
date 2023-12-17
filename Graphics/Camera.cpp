#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 startingPosition, glm::vec3 cameraTarget, MouseInput* mouseInput)
	: m_CameraPosition(startingPosition),
	m_CameraTarget(cameraTarget),
	m_ViewMatrix(glm::mat4(1.0f)),
	m_MouseInput(mouseInput),
	m_Pitch(0.0f),
	m_Yaw(-90.0f)
{
	m_CameraRight = glm::normalize(glm::cross(m_CameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraTarget));

	m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraTarget, m_CameraUp);
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraTarget, m_CameraUp);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
}

void Camera::ProcessInput(GLFWwindow* window, double deltaTime)
{
	const float cameraSpeed = 2.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_CameraPosition = m_CameraPosition + cameraSpeed * m_CameraTarget;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_CameraPosition = m_CameraPosition - cameraSpeed * m_CameraTarget;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_CameraPosition = m_CameraPosition - cameraSpeed * m_CameraRight;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_CameraPosition = m_CameraPosition + cameraSpeed * m_CameraRight;


	float offsetX = m_MouseInput->GetOffsetX();
	float offsetY = m_MouseInput->GetOffsetY();
	//std::cout << "GetOffsetX = " << offsetX << std::endl;
	//std::cout << "GetOffsetY = " << offsetY << std::endl << std::endl;


	if (offsetX != 0 || offsetY != 0)
	{
		float mouseSensitivity = 0.1f;
		offsetX *= mouseSensitivity;
		offsetY *= mouseSensitivity;
		m_Yaw += offsetX;
		m_Pitch += offsetY;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
		glm::vec3 camTarget;
		camTarget.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		camTarget.y = sin(glm::radians(m_Pitch));
		camTarget.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_CameraTarget = glm::normalize(camTarget);

		m_CameraRight = glm::normalize(glm::cross(m_CameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraTarget));
	}

}

glm::vec3 Camera::GetPostion()
{
	return m_CameraPosition;
}
