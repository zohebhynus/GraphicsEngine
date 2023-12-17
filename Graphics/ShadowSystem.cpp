#include <format>

#include "ShadowSystem.h"

void renderQuad();

ShadowSystem::ShadowSystem(Shader* dirLightShadowMapShader, Shader* pointLightShadowCubeMapShader, LightSystem* lightSystem)
	:m_DirLightShadowMapShader(dirLightShadowMapShader),
	m_PointLightShadowCubeMapShader(pointLightShadowCubeMapShader),
	m_LightSystem(lightSystem),
	m_DirLightShadowMapWidth(4096),
	m_DirLightShadowMapHeight(4096)
{
	SetUpDirectionalLightShadow();
	SetUpPointLightShadow();
}

void ShadowSystem::GenerateDirLightShadowMap(std::vector<GameObject>& objectList)
{
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_DirLightShadowMapWidth, m_DirLightShadowMapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DirLightShadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(-m_LightSystem->GetDirectionalLight().Direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_LightProjection = orthgonalProjection * lightView;
	m_DirLightShadowMapShader->Bind();
	m_DirLightShadowMapShader->SetUniformMatrix4fv("lightProjection", m_LightProjection);
	for (GameObject& obj : objectList)
	{

		obj.DepthDraw(m_DirLightShadowMapShader);
	}
	m_DirLightShadowMapShader->UnBind();

	// Switch back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowSystem::RenderDirLightDebugQuad()
{
	m_DirLightShadowMapShader->Bind();
	m_DirLightShadowMapShader->SetUniform1f("near_plane", 0.1f);
	m_DirLightShadowMapShader->SetUniform1f("far_plane", 75.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DirLightShadowMapTexture);
	renderQuad();
}

void ShadowSystem::SetShaderValuesInLightShader(Shader* lightShader)
{
	lightShader->Bind();
	lightShader->SetUniformMatrix4fv("lightProjection", m_LightProjection);
	lightShader->SetUniform1f("farPlane", far_value);

	glActiveTexture(GL_TEXTURE0 + 8);
	lightShader->SetUniform1i("shadowMap", 8);
	glBindTexture(GL_TEXTURE_2D, m_DirLightShadowMapTexture);

	for (int i = 0; i < m_LightSystem->GetPointLightList().size(); i++)
	{
		glActiveTexture(GL_TEXTURE9 + i);
		lightShader->SetUniform1i(std::format("shadowCubeMap[{0}]", i), 9 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightShadowCubeMapTexture[i]);
	}
	lightShader->UnBind();
}

void ShadowSystem::GeneratePointLightShadowCubeMap(std::vector<GameObject>& objectList)
{
	float aspect = (float)m_DirLightShadowMapWidth / (float)m_DirLightShadowMapHeight;
	float near = 0.1f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far_value);

	for (int i = 0; i < m_LightSystem->GetPointLightList().size(); i++)
	{
		PointLight li = m_LightSystem->GetPointLightList()[i];
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(li.Position, li.Position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


		glViewport(0, 0, m_DirLightShadowMapWidth, m_DirLightShadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, m_PointLightShadowMapFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_PointLightShadowCubeMapShader->Bind();

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_PointLightShadowCubeMapShader->SetUniformMatrix4fv(std::format("shadowMatrices[{0}]", i), shadowTransforms[i]);
		}

		m_PointLightShadowCubeMapShader->SetUniform1f("farPlane", far_value);
		m_PointLightShadowCubeMapShader->SetUniform4f("lightPos", li.Position.x, li.Position.y, li.Position.z, 1.0f);

		for (GameObject& obj : objectList)
		{
			obj.DepthDraw(m_PointLightShadowCubeMapShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ShadowSystem::SetUpDirectionalLightShadow()
{
	//Dir Light Shadow code
	glGenFramebuffers(1, &m_DirLightShadowMapFBO);

	glGenTextures(1, &m_DirLightShadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_DirLightShadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_DirLightShadowMapWidth, m_DirLightShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_DirLightShadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DirLightShadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(-m_LightSystem->GetDirectionalLight().Direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_LightProjection = orthgonalProjection * lightView;
}

void ShadowSystem::SetUpPointLightShadow()
{

	for (int i = 0; i < m_LightSystem->GetPointLightList().size(); i++)
	{
		//Point Light Shadow code
		unsigned int ptLightShadowMapFBO;
		glGenFramebuffers(1, &ptLightShadowMapFBO);

		unsigned int ptLightShadowMapTexture;
		glGenTextures(1, &ptLightShadowMapTexture);

		glBindTexture(GL_TEXTURE_CUBE_MAP, ptLightShadowMapTexture);
		for (unsigned int j = 0; j < 6; ++j)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT,
				m_DirLightShadowMapWidth, m_DirLightShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, ptLightShadowMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ptLightShadowMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_PointLightShadowMapFBO.push_back(ptLightShadowMapFBO);
		m_PointLightShadowCubeMapTexture.push_back(ptLightShadowMapTexture);
	}


}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
