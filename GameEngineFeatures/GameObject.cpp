#include "GameObject.h"

GameObject::GameObject(const char* name, Model* model, Shader* shader)
    :m_Name(name),
    m_Model(model),
    m_Shader(shader)
{
}

std::string GameObject::GetName()
{
    return m_Name;
}

Transform& GameObject::GetTransform()
{
    return m_Transform;
}

void GameObject::SetName(std::string newName)
{
    m_Name = newName;
}

void GameObject::Draw(Camera& camera)
{
    if (m_Shader != nullptr)
    {
        m_Shader->Bind();
        m_Shader->SetUniformMatrix4fv("model", m_Transform.GetModelMatrix());
        m_Shader->SetUniformMatrix4fv("projection", camera.GetProjectionMatrix());
        m_Shader->SetUniformMatrix4fv("view", camera.GetViewMatrix());
        m_Shader->SetUniform4f("viewPos", camera.GetPostion().x, camera.GetPostion().y, camera.GetPostion().z, 1.0);
        m_Shader->SetUniformMatrix4fv("inverseTransposeModel", m_Transform.GetTransposeInverseModelMatrix());

        m_Model->Draw(m_Shader);
    }
    else
    {
        std::cout << " Shader is nullptr for Model :" << m_Name << std::endl;
    }
}

void GameObject::DepthDraw(Shader* shadowMapShader)
{
    shadowMapShader->SetUniformMatrix4fv("model", m_Transform.GetModelMatrix());
    m_Model->Draw(shadowMapShader);
}
