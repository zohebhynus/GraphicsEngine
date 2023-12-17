#include "LightSystem.h"
#include <format>
#include<iostream>

#include "../Utilities/Random.h"
#include "Shader.h"

LightSystem::LightSystem(Shader* lightShader, Shader* lightObjectShader, Model* lightObjectModel)
	: m_LightShader(lightShader),
	m_LightObjectShader(lightObjectShader),
	m_LightObjectModel(lightObjectModel)
{
	m_AmbientLight.LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	m_AmbientLight.ColorIntensity = 0.03;

	m_DirectionalLight.Direction = glm::vec3(-25.712f, -30.642f, 0.0f);
	m_DirectionalLight.LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	m_DirectionalLight.ColorIntensity = 0.4f;
	m_DirectionalLight.SpecularIntensity = 1.0f;


	PointLight bigLight;
	bigLight.Name = "Big Light";
	bigLight.LightColor = glm::vec3(1.0f, 0.0f, 0.0f);
	bigLight.Position = glm::vec3(0.0f, 4.0f, 0.0f);
	bigLight.radius = 10.0f;
	bigLight.edgeIntensity = 9.0f;

	m_PointLights.push_back(bigLight);

	std::string name = "Light";
	for (int i = 0; i < NUMBER_OF_LIGHTS - 1; i++)
	{
		PointLight light;
		light.Name = name + std::to_string(i);
		light.LightColor = glm::vec3(Random::Float() * 1.0f, Random::Float() * 1.0f, Random::Float() * 1.0f);
		light.Position = glm::vec3((Random::Float() * 20.0f) - 10.0f, 1.5f, (Random::Float() * 20.0f) - 10.f);
		light.radius = 4.0f;
		light.edgeIntensity = 3.0f;

		m_PointLights.push_back(light);
	}







	//PointLight pointLightOne, pointLightTwo, pointLightThree, pointLightFour;
	//pointLightOne.Name = "Red";
	//pointLightOne.LightColor = glm::vec3(1.0f, 0.0f, 0.0f);
	//pointLightOne.Position = glm::vec3(0.0f, 5.0f, 6.0f);
	//pointLightOne.radius = 4.0f;
	//pointLightOne.edgeIntensity = 2.0f;

	//pointLightTwo.Name = "Green";
	//pointLightTwo.LightColor = glm::vec3(0.0f, 1.0f, 0.0f);
	//pointLightTwo.Position = glm::vec3(-2.0f, 1.0f, 2.0f);
	//pointLightTwo.radius = 4.0f;
	//pointLightTwo.edgeIntensity = 2.0f;

	//pointLightThree.Name = "Blue";
	//pointLightThree.LightColor = glm::vec3(0.0f, 0.0f, 1.0f);
	//pointLightThree.Position = glm::vec3(-2.0f, 1.0f, -2.0f);
	//pointLightThree.radius = 4.0f;
	//pointLightThree.edgeIntensity = 2.0f;

	//pointLightFour.Name = "White";
	//pointLightFour.LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	//pointLightFour.Position = glm::vec3(2.0f, 1.0f, -2.0f);
	//pointLightFour.radius = 4.0f;
	//pointLightFour.edgeIntensity = 2.0f;

	//m_PointLights.push_back(pointLightOne);
	//m_PointLights.push_back(pointLightTwo);
	//m_PointLights.push_back(pointLightThree);
	//m_PointLights.push_back(pointLightFour);
}

void LightSystem::SetLightData()
{
	m_LightShader->Bind();

	if (m_AmbientLight.IsEnabled)
	{
		m_LightShader->SetUniform1f("ambLight.colorIntensity", m_AmbientLight.ColorIntensity);
	}
	else
	{
		m_LightShader->SetUniform1f("ambLight.colorIntensity", 0.0f);
	}

	m_LightShader->SetUniform4f("ambLight.lightColor", m_AmbientLight.LightColor.x, m_AmbientLight.LightColor.y, m_AmbientLight.LightColor.z, 1.0f);


	if (m_DirectionalLight.IsEnabled)
	{
		m_LightShader->SetUniform1f("dirLight.colorIntensity", m_DirectionalLight.ColorIntensity);
		m_LightShader->SetUniform1f("dirLight.specularIntensity", m_DirectionalLight.SpecularIntensity);
	}
	else
	{
		m_LightShader->SetUniform1f("dirLight.colorIntensity", 0.0f);
		m_LightShader->SetUniform1f("dirLight.specularIntensity", 0.0f);

	}

	m_LightShader->SetUniform4f("dirLight.direction", m_DirectionalLight.Direction.x, m_DirectionalLight.Direction.y, m_DirectionalLight.Direction.z, 1.0f);
	m_LightShader->SetUniform4f("dirLight.lightColor", m_DirectionalLight.LightColor.x, m_DirectionalLight.LightColor.y, m_DirectionalLight.LightColor.z, 1.0f);

	for (int i = 0; i < m_PointLights.size(); i++)
	{
		m_LightShader->SetUniform4f(std::format("ptLights[{0}].position", i), m_PointLights[i].Position.x, m_PointLights[i].Position.y, m_PointLights[i].Position.z, 1.0f);
		m_LightShader->SetUniform4f(std::format("ptLights[{0}].lightColor", i), m_PointLights[i].LightColor.x, m_PointLights[i].LightColor.y, m_PointLights[i].LightColor.z, 1.0f);
		
		m_LightShader->SetUniform4f(std::format("ptLights[{0}].radiusIntensity", i), 0.0f, 0.0f, m_PointLights[i].radius, m_PointLights[i].edgeIntensity);

		//m_LightShader->SetUniform1f(std::format("ptLights[{0}].radius", i), m_PointLights[i].radius);
		//m_LightShader->SetUniform1f(std::format("ptLights[{0}].edgeIntensity", i), m_PointLights[i].edgeIntensity);
	}

	m_LightShader->UnBind();
}

GlobalAmbientLight LightSystem::GetAmbientLight()
{
	return m_AmbientLight;
}

void LightSystem::SetAmbientLight(GlobalAmbientLight ambientLight)
{
	m_AmbientLight.ColorIntensity = ambientLight.ColorIntensity;
	m_AmbientLight.LightColor = ambientLight.LightColor;
	m_AmbientLight.IsEnabled = ambientLight.IsEnabled;
}

DirectionalLight LightSystem::GetDirectionalLight()
{
	return m_DirectionalLight;
}

void LightSystem::SetDirectionalLight(DirectionalLight dirLight)
{
	m_DirectionalLight = dirLight;
}

std::vector<PointLight> LightSystem::GetPointLightList()
{
	return m_PointLights;
}

void LightSystem::SetPointLight(std::string name, PointLight pointLight)
{
	for (int i = 0; i < m_PointLights.size(); i++)
	{
		if (m_PointLights[i].Name == name)
		{
			m_PointLights[i].Position = pointLight.Position;
			m_PointLights[i].LightColor = pointLight.LightColor;
			m_PointLights[i].radius = pointLight.radius;
			m_PointLights[i].edgeIntensity = pointLight.edgeIntensity;
			break;
		}
	}

}

void LightSystem::DrawLightObjects(Camera& camera)
{
	if (m_LightObjectShader != nullptr)
	{
		m_LightObjectShader->Bind();
		for (int i = 0; i < m_PointLights.size(); i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, m_PointLights[i].Position);
			model = glm::scale(model, glm::vec3(0.1f));
			m_LightObjectShader->SetUniformMatrix4fv("model", model);
			m_LightObjectShader->SetUniformMatrix4fv("projection", camera.GetProjectionMatrix());
			m_LightObjectShader->SetUniformMatrix4fv("view", camera.GetViewMatrix());
			m_LightObjectShader->SetUniform4f("lightColor", m_PointLights[i].LightColor.x, m_PointLights[i].LightColor.y, m_PointLights[i].LightColor.z, 1.0);

			m_LightObjectModel->Draw(m_LightObjectShader);
		}

		//directionalLight
		glm::mat4 model(1.0f);
		model = glm::translate(model, -m_DirectionalLight.Direction);
		if (m_DirectionalLight.IsEnabled)
			model = glm::scale(model, glm::vec3(5.0f));
		else
			model = glm::scale(model, glm::vec3(2.0f));

		m_LightObjectShader->SetUniformMatrix4fv("model", model);
		m_LightObjectShader->SetUniform4f("lightColor", m_DirectionalLight.LightColor.x, m_DirectionalLight.LightColor.y, m_DirectionalLight.LightColor.z, 1.0);
		m_LightObjectModel->Draw(m_LightObjectShader);

		m_LightObjectShader->UnBind();
	}
	else
	{
		std::cout << " Shader is nullptr for LightObjectModel" << std::endl;
	}
}

glm::vec4 LightSystem::GetEnvironmentColor()
{
	if (m_DirectionalLight.IsEnabled)
		return glm::vec4(0.369f, 0.808f, 1.0f, 1.0f);
	else
		return glm::vec4(0.18f, 0.078f, 0.271f, 1.0f);

}
