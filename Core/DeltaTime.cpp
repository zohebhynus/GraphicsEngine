#include "DeltaTime.h"

#include <GLFW/glfw3.h>

DeltaTime::DeltaTime() :
	m_LastFrameTime(glfwGetTime()), 
	m_DeltaTime(0.0)
{
}

DeltaTime::~DeltaTime()
{
}

void DeltaTime::CalculateDeltaTime()
{
	double currentFrameTime = glfwGetTime();
	m_DeltaTime = currentFrameTime - m_LastFrameTime;
	m_LastFrameTime = currentFrameTime;
}

double DeltaTime::GetDeltaTime()
{
	return m_DeltaTime;
}
