#include "Transform.h"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    : m_CurrentPosition(position),
    m_CurrentRotation(rotation),
    m_CurrentScale(scale),
    m_ModelMatrix(glm::mat4(1.0f)),
    m_TransposeInverseModelMatrix(glm::mat4(1.0f))
{
    CalculateModelMatrix();
}

glm::vec3 Transform::GetPosition()
{
    return m_CurrentPosition;
}

glm::vec3 Transform::GetRotation()
{
    return m_CurrentRotation;
}

glm::vec3 Transform::GetScale()
{
    return m_CurrentScale;
}

glm::mat4 Transform::GetModelMatrix()
{
    return m_ModelMatrix;
}

glm::mat4 Transform::GetTransposeInverseModelMatrix()
{
    return m_TransposeInverseModelMatrix;
}

void Transform::SetPosition(glm::vec3 newPosition)
{
    m_CurrentPosition = newPosition;
    CalculateModelMatrix();
}

void Transform::SetRotation(float angleInDegrees, glm::vec3 axis)
{
    if (axis.x)
    {
        m_CurrentRotation.x = angleInDegrees;
    }
    else if (axis.y)
    {
        m_CurrentRotation.y = angleInDegrees;
    }
    else
    {
        m_CurrentRotation.z = angleInDegrees;
    }
    CalculateModelMatrix();
}

void Transform::SetScale(glm::vec3 newScale)
{
    m_CurrentScale = newScale;
    CalculateModelMatrix();
}

void Transform::CalculateModelMatrix()
{
    m_ModelMatrix = glm::mat4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_CurrentPosition);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_CurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_CurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_CurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_CurrentScale);

    m_TransposeInverseModelMatrix = glm::transpose(glm::inverse(m_ModelMatrix));
}