#include "crystalpch.h"
#include "crystal/renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Crystal
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		CRYSTAL_PROFILE_FUNCTION();

		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void OrthographicCamera::SetPosition(const vec3& position)
	{
		CRYSTAL_PROFILE_FUNCTION();

		this->position = position;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(float rotation)
	{
		CRYSTAL_PROFILE_FUNCTION();

		this->rotation = rotation;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		CRYSTAL_PROFILE_FUNCTION();

		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		CRYSTAL_PROFILE_FUNCTION();

		mat4 transform = translate(mat4(1.0f), position) * rotate(mat4(1.0f), radians(rotation), vec3(0, 0, 1));
		
		viewMatrix = inverse(transform);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}
}