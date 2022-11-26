#pragma once
#include "definitions.h"
#include "Texture.h"
#include <vector>
#include "rendering/buffer.h"

namespace hyv
{
	namespace rendering
	{
		struct camera
		{
			dl::RefCntAutoPtr<dl::ITexture> albedo_buffer;
			dl::RefCntAutoPtr<dl::ITexture> normal_buffer;
			dl::RefCntAutoPtr<dl::ITexture> depth_buffer;
			int width, height;
			float clear_color[4] = { 0,0,0,1 };
			glm::mat4 projection;
		};

		struct geometry_pass_VS_per_mesh_constants
		{
			glm::mat4 model; // 4x4 * sizeof(float) = 16 * 4
			glm::mat4 normal; // 4 * 4 = 16 * 4
			glm::mat4 MVP; // 4 * 4  = 16 * 4  
		};

		typedef std::vector<uniform_buffer<geometry_pass_VS_per_mesh_constants>> geometry_pass_constants_vector;

	}
}