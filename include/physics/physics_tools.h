#pragma once
#include "physics/physics_components.h"
#include <utility>
#include "glm/mat4x4.hpp"
#include <algorithm>
#include "logging.h"


#define WORLD_UP glm::vec3{0.f,-1.f,0.f}
#define WORLD_FWD glm::vec3{0.f,0.f,1.f}
#define WORLD_RGT glm::vec3{1.f,0.f,0.f}

namespace hyv
{
	namespace physics
	{
		inline glm::vec3 spherical_to_cartesian(const glm::vec2& spherical)
		{
			glm::vec3 forward;
			auto rad = glm::radians(spherical);
			forward.x = sin(rad.x) * cos(rad.y);
			forward.z = sin(rad.x) * sin(rad.y);
			forward.y = cos(rad.x);
			return normalize(forward);
		}

		inline std::pair<glm::mat4, glm::mat4> get_model_normal(const transform& trans)
		{
			glm::mat4 m{ 1.0 };
			return { m,m };
			auto translationM = glm::translate(m, trans.position);
			auto rads = glm::radians(trans.rotation);
			auto rotM = glm::yawPitchRoll(rads.x, rads.y, rads.z);
			auto scaleM = glm::scale(m, trans.scale);
			auto model = translationM * rotM * scaleM;
			auto normal = glm::inverse(glm::transpose(model));
			return { model,normal };
		}
		//inline glm::vec3 rotate_vector(const glm::vec2& rotation, const glm::vec3& vector)
		//{
		//	float thetaNew = acos(vector.y) + glm::radians(rotation.x);
		//	float phiNew = atan2(vector.z, vector.x) + glm::radians(rotation.y);


		//	thetaNew = std::clamp(thetaNew, 0.f, 3.14f);
		//	phiNew = fmod(phiNew, (3.14f * 2));
		//	return spherical_to_cartesian({ thetaNew,phiNew });
		//}

		inline void rotate(transform& trans, const glm::vec3& rotation)
		{
			trans.rotation += rotation;
		}



		inline glm::vec3 get_forward(const transform& trans)
		{
			auto rads = glm::radians(trans.rotation);
			auto f= glm::vec3{ sin(rads.x) * cos(rads.y), -glm::sin(rads.y),glm::cos(rads.x) * glm::cos(rads.y) };
			return glm::normalize(f);
		}

		inline glm::mat4 get_view(const transform& trans)
		{
			return glm::lookAtLH(trans.position, trans.position + get_forward(trans), WORLD_UP);
		}

		inline void move(
			transform& trans,
			const glm::vec3& delta)
		{
			auto forward = get_forward(trans);
			if (abs(delta.x) <= 0.00000001 && abs(delta.y) <= 0.00000001 && abs(delta.z) <= 0.00000001) return;
			float len = glm::length(delta);
			// delta is in local space -> transform it to view space
			// compute right:
			auto right = glm::cross(forward, WORLD_UP);
			// compute up:
			auto up = glm::cross(right, forward);
			// compute new coord system
			glm::mat3 viewCoord(right, up, forward);
			trans.position = trans.position + glm::normalize(viewCoord * glm::normalize(delta)) * len;
		}
	}
}