#pragma once
#include <cassert>
#include <algorithm>
#include "TimeManager.h"
#include <glm/glm.hpp>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
	Camera() = default;

	glm::vec3 m_Origin{};
	float m_FovAngle{90.f };

    constexpr static glm::vec3 UP{0.f, 1.f, 0.f};
    constexpr static glm::vec3 FORWARD{0.f, 0.f, 1.f};
    constexpr static glm::vec3 RIGHT{1.f, 0.f, 0.f};

	glm::vec3 m_Forward{FORWARD};
	glm::vec3 m_Up{UP};
	glm::vec3 m_Right{RIGHT};

	float m_TotalPitch{};
	float m_TotalYaw{};

	glm::mat4x4 m_ViewMatrix{};
	glm::mat4x4 m_ProjectionMatrix{};

	const float m_Near{ .1f };
	const float m_Far{ 1000.f };
    float m_RotationSpeed{ 20.f };
    float m_MovementSpeed{10.f};

	void Initialize(float _fovAngle = 90.f, glm::vec3 _origin = { 0.f,0.f,0.f }, float _aspectRatio = 1.f)
	{
        m_FovAngle = glm::radians(_fovAngle);
        CalculateProjectionMatrix(_aspectRatio);
        m_Origin = _origin;
	}

	void CalculateViewMatrix()
	{
        m_ViewMatrix = glm::lookAtLH(m_Origin  , m_Origin + m_Forward, m_Up);
	}

	void CalculateProjectionMatrix(float aspectRatio)
	{
        m_ProjectionMatrix = glm::perspectiveLH(m_FovAngle, aspectRatio, m_Near, m_Far);
        m_ProjectionMatrix[1][1] *= -1;
	}

	void Update()
	{
		glm::mat4x4 finalRotation{};

        finalRotation = glm::rotate(glm::mat4x4(1.f), glm::radians(m_TotalPitch), m_Right);
        finalRotation *= glm::rotate(glm::mat4x4(1.f), glm::radians(m_TotalYaw), {0, 1, 0});

        m_Forward = glm::vec3(glm::normalize(finalRotation * glm::vec4{0, 0, 1, 0}));
        m_Right = glm::normalize(glm::cross({0, 1, 0}, m_Forward));
        m_Up = glm::normalize(glm::cross(m_Forward, m_Right));
		//Update Matrices
		CalculateViewMatrix();
	}
};
