#pragma once
#include "physics/physics_components.h"
#include <utility>
#include "glm/mat4x4.hpp"


namespace hyv
{
	namespace physics
	{
		std::pair<glm::mat4, glm::mat4> get_model_normal(const position& pos, const rotation& rot, const scale& s);
	}
}