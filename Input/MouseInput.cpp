#include "MouseInput.h"
#include <iostream>

MouseInput::MouseInput(unsigned int width, unsigned int height, GLFWwindow* window)
	: m_LastX(width / 2.0f),
	m_LastY(height / 2.0f),
	m_CurrentX(0.0f),
	m_CurrentY(0.0f),
	m_Window(window),
	m_OffsetX(0.0f),
	m_OffsetY(0.0f)
{
}

MouseInput::~MouseInput()
{
}

float MouseInput::GetOffsetX()
{
	return m_OffsetX;
}

float MouseInput::GetOffsetY()
{
	return m_OffsetY;
}

void MouseInput::Update()
{
	m_OffsetX = m_CurrentX - m_LastX;
	m_OffsetY = m_CurrentY - m_LastY;

	m_LastX = m_CurrentX;
	m_LastY = m_CurrentY;
}

void MouseInput::MousePositionCallBack(GLFWwindow* window, double inputPositionX, double inputPositionY)
{
	MouseInput* mouseInput = static_cast<MouseInput*>(glfwGetWindowUserPointer(window));

	mouseInput->m_CurrentX = static_cast<float>(inputPositionX);
	mouseInput->m_CurrentY = static_cast<float>(-inputPositionY); //idk why it is returning inverted value, so making it negative to reinvert
}
