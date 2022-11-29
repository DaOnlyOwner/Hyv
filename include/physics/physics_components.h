#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace hyv
{
	namespace physics
	{
		struct transform
		{
			glm::vec3 position = { 0,0,0 };
			glm::vec3 rotation = { 0,0,0 };
			glm::vec3 scale = { 1,1,1 };
		};

		struct main_camera_transform
		{
			transform trans;
		};
	}
}
