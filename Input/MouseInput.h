#pragma once
#include<GLFW/glfw3.h>
class MouseInput
{
public:
	MouseInput(unsigned int width, unsigned int height, GLFWwindow* window);
	~MouseInput();

	float GetOffsetX();
	float GetOffsetY();

	void Update();

	static void MousePositionCallBack(GLFWwindow* window, double inputPositionX, double inputPositionY);

private:
	GLFWwindow* m_Window;
	float m_LastX;
	float m_LastY;

	float m_CurrentX;
	float m_CurrentY;

	float m_OffsetX;
	float m_OffsetY;
};