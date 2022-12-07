#pragma once
#include "definitions.h"
#include "Texture.h"
#include <vector>
#include "rendering/buffer.h"
#include "pipeline.h"

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

		struct geometry_pass_constants
		{
			glm::mat4 model; // 4x4 * sizeof(float) = 16 * 4
			glm::mat4 normal; // 4 * 4 = 16 * 4
			glm::mat4 MVP; // 4 * 4  = 16 * 4  
		};

		struct draw_indirect_command
		{
			u32 num_indices;
			u32 num_instances;
			u32 first_index_location;
			u32 base_vertex;
			u32 first_instance_location;
		};

		struct model_normal_bundle
		{
			glm::mat4 model;
			glm::mat4 normal;
		};

		typedef struct_buffer<model_normal_bundle> object_data_buffer;
		typedef struct_buffer<draw_indirect_command> indirect_draws_buffer;
		
		typedef std::vector<uniform_buffer<geometry_pass_constants>> geometry_pass_constants_vector;

		
		struct geometry_pass_pipeline_bundle
		{
			graphics_pipeline pso;
			dl::RefCntAutoPtr<dl::IShaderResourceBinding> SRB;
			uniform_buffer<geometry_pass_constants> consts;
		};

		struct composite_pass_pipeline_bundle
		{
			graphics_pipeline pso;
			dl::RefCntAutoPtr<dl::IShaderResourceBinding> SRB;
		};

		struct global_mesh_buffer
		{
			dl::RefCntAutoPtr<dl::IBuffer> vertex_buffer;
			dl::RefCntAutoPtr<dl::IBuffer> index_buffer;
		};

		struct MainCameraTag {};

	}
}