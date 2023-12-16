#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include "DeltaTime.h"


class Application
{
public:
	int Initialize();
	void Run();
	void Release();

private:
	GLFWwindow* m_Window;
	DeltaTime dt;

};
