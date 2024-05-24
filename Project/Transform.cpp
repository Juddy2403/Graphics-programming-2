#include <glm/ext/matrix_transform.hpp>
#include "Transform.h"
#include "Camera.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

void Transform::Translate(const glm::vec3 &translation) {
    m_TranslationMatrix = glm::translate(m_TranslationMatrix, translation);
    m_NeedsUpdate = true;
}

void Transform::Rotate(const glm::vec3 &rotation) {
    m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(rotation.x), Camera::RIGHT);
    m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(rotation.y), Camera::UP);
    m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(rotation.z), Camera::FORWARD);

    m_NeedsUpdate = true;
}

void Transform::Scale(const glm::vec3 &scale) {
    m_ScaleMatrix = glm::scale(m_ScaleMatrix, scale);

    m_NeedsUpdate = true;
}

glm::mat4 Transform::GetWorldMatrix() {
    if(m_NeedsUpdate) {
        m_WorldMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
        m_NeedsUpdate = false;
    }
    return m_WorldMatrix;
}

void Transform::Update() {
    if(m_RotationPerSecond != glm::vec3(0.f)) {
        Rotate(m_RotationPerSecond* TimeManager::GetInstance().GetElapsed());
    }
}

void Transform::SetRotationPerSecond(const glm::vec3 &rotationPerSecond) {
    m_RotationPerSecond = rotationPerSecond;
}
