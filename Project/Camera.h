#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include <glm/gtc/constants.hpp> // For glm::pi
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
	Camera() = default;

	Camera(const glm::vec3& _origin, float _fovAngle)
	{
		Initialize(_fovAngle, origin);
		CalculateProjectionMatrix();
	}

	glm::vec3 origin{};
	float fovAngle{ 90.f };

	glm::vec3 forward{ glm::vec3(0.0f, 0.0f, 1.0f) };
	glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
	glm::vec3 right{ glm::vec3(1.0f, 0.0f, 0.0f) };

	float totalPitch{};
	float totalYaw{};

	glm::mat4x4 viewMatrix{};
	glm::mat4x4 projectionMatrix{};

	const float m_Near{ .1f };
	const float m_Far{ 1000.f };
	float aspectRatio{};
	float screenWidth{};
	float screenHeight{};
	float fov{};

	void Initialize(float _fovAngle = 90.f, glm::vec3 _origin = { 0.f,0.f,0.f }, float _aspectRatio = 1.f)
	{
		fovAngle = _fovAngle;
		fov = tanf(glm::radians(_fovAngle) / 2.f);
		aspectRatio = _aspectRatio;
		CalculateProjectionMatrix();
		origin = _origin;
	}

	void CalculateViewMatrix()
	{
		viewMatrix = glm::lookAtLH(origin, forward, up);
		viewMatrix = glm::inverse(viewMatrix);
	}

	void CalculateProjectionMatrix()
	{
		//float fov = tanf((fovAngle * TO_RADIANS) / 2.f);
		//fov should be in rad
		projectionMatrix = glm::perspectiveFovLH(fov, screenWidth, screenHeight, m_Near, m_Far);
	}

	void Update(const Timer* pTimer)
	{
		//Camera Update Logic
		float deltaTime = pTimer->GetElapsed();
		deltaTime = std::clamp(deltaTime, 0.005f, 0.01f);

		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

		//Mouse Input
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		const float rotationSpeed{ 40.f };
		float movementSpeed{};

		if (pKeyboardState[SDL_SCANCODE_LSHIFT]) movementSpeed = 12.f;
		else movementSpeed = 6.f;
		if (pKeyboardState[SDL_SCANCODE_W]) origin += (movementSpeed * deltaTime) * forward.Normalized();
		if (pKeyboardState[SDL_SCANCODE_S]) origin -= (movementSpeed * deltaTime) * forward.Normalized();
		if (pKeyboardState[SDL_SCANCODE_A]) origin -= (movementSpeed * deltaTime) * right.Normalized();
		if (pKeyboardState[SDL_SCANCODE_D]) origin += (movementSpeed * deltaTime) * right.Normalized();

		if (mouseState == SDL_BUTTON_LEFT && mouseY)	origin -= (movementSpeed * deltaTime) * forward.Normalized() * mouseY;
		if (mouseState == SDL_BUTTON_X2 && mouseY) origin -= (movementSpeed * deltaTime) * up.Normalized() * mouseY;
		if (mouseState == SDL_BUTTON_LEFT && mouseX) totalYaw += rotationSpeed * deltaTime * mouseX;

		if (mouseState == SDL_BUTTON_X1)
		{
			if (!(totalPitch > 88.f && mouseY <= 0) && !(totalPitch < -88.f && mouseY >= 0))
				totalPitch -= rotationSpeed * deltaTime * mouseY;

			totalYaw += rotationSpeed * deltaTime * mouseX;
		}
		Matrix finalRotation{};

		finalRotation = Matrix::CreateRotationX(totalPitch * TO_RADIANS);
		finalRotation *= Matrix::CreateRotationY(totalYaw * TO_RADIANS);

		forward = finalRotation.TransformVector(glm::vec3::UnitZ);
		forward.Normalize();
		right = glm::vec3::Cross(glm::vec3::UnitY, forward).Normalized();
		up = glm::vec3::Cross(forward, right).Normalized();
		//Update Matrices
		CalculateViewMatrix();
	}
};

