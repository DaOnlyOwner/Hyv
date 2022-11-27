#pragma once
#include "definitions.h"
#include "Texture.h"
#include <vector>
#include "rendering/buffer.h"

namespace hyv
{
	namespace rendering
	{

		struct camera_constants
		{
			glm::mat4 VPInv;
			glm::vec4 eyePos;
		};


		struct camera
		{
			dl::RefCntAutoPtr<dl::ITexture> albedo_buffer;
			dl::RefCntAutoPtr<dl::ITexture> normal_buffer;
			dl::RefCntAutoPtr<dl::ITexture> depth_buffer;
			//uniform_buffer<camera_constants> constants;

			int width, height;
			float clear_color[4] = { 0,0,0,1 };
			glm::mat4 projection;
		};

		struct material
		{
			dl::RefCntAutoPtr<dl::IPipelineState> pipeline;
			dl::RefCntAutoPtr<dl::IShaderResourceBinding> SRB;
		};


	}
}