#pragma once
#include "glm/mat4x4.hpp"
#include <vector>
#include "rendering/buffer.h"


namespace hyv
{
	namespace rendering
	{
		struct geometry_pass_VS_per_mesh_constants
		{
			glm::mat4 model; // 4x4 * sizeof(float) = 16 * 4
			glm::mat4 normal; // 4 * 4 = 16 * 4
			glm::mat4 MVP; // 4 * 4  = 16 * 4  
		};

		typedef std::vector<uniform_buffer<geometry_pass_VS_per_mesh_constants>> geometry_pass_constants_vector;

		struct geometry_pass_ctxt
		{
			geometry_pass_constants_vector constants;
		};
	}
}
