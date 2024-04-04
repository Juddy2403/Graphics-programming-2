#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <algorithm>
#include "TimeManager.h"
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
		fovAngle = glm::radians(_fovAngle);
		fov = tanf(fovAngle / 2.f);
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
		projectionMatrix = glm::perspectiveFovLH(fovAngle, screenWidth, screenHeight, m_Near, m_Far);
	}

	void Update()
	{
		//Camera Update Logic
		float deltaTime = TimeManager::GetElapsed();
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
		if (pKeyboardState[SDL_SCANCODE_W]) origin += (movementSpeed * deltaTime) * glm::normalize(forward);
		if (pKeyboardState[SDL_SCANCODE_S]) origin -= (movementSpeed * deltaTime) * glm::normalize(forward);
		if (pKeyboardState[SDL_SCANCODE_A]) origin -= (movementSpeed * deltaTime) * glm::normalize(right);
		if (pKeyboardState[SDL_SCANCODE_D]) origin += (movementSpeed * deltaTime) * glm::normalize(right);

		if (mouseState == SDL_BUTTON_LEFT && mouseY)	origin -= (movementSpeed * deltaTime* static_cast<float>(mouseY)) * glm::normalize(forward) ;
		if (mouseState == SDL_BUTTON_X2 && mouseY) origin -= (movementSpeed * deltaTime) * glm::normalize(up) * static_cast<float>(mouseY);
		if (mouseState == SDL_BUTTON_LEFT && mouseX) totalYaw += rotationSpeed * deltaTime * static_cast<float>(mouseX);

		if (mouseState == SDL_BUTTON_X1)
		{
			if (!(totalPitch > 88.f && mouseY <= 0) && !(totalPitch < -88.f && mouseY >= 0))
				totalPitch -= rotationSpeed * deltaTime * static_cast<float>(mouseY);

			totalYaw += rotationSpeed * deltaTime * static_cast<float>(mouseX);
		}
		glm::mat4x4 finalRotation{};

        finalRotation = glm::rotate(glm::mat4x4{},glm::radians(totalPitch),{1,0,0} );
        finalRotation *= glm::rotate(glm::mat4x4{},glm::radians(totalYaw),{0,1,0} );


        forward = glm::normalize(finalRotation * glm::vec4{0,0,1,1});
        right = glm::normalize(glm::cross({0,1,0},forward));
        up = glm::normalize(glm::cross(forward,right));
		//Update Matrices
		CalculateViewMatrix();
	}
};

