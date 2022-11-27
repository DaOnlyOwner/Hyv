#pragma once
#include "global.h"
#include "shader.h"
#include <type_traits>

namespace hyv
{
	namespace rendering
	{
		enum class pipeline_type
		{
			Graphics,Compute
		};

		class graphics_pipeline
		{
		public:
			graphics_pipeline& setup_geometry_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size);
			graphics_pipeline& setup_composite_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size);
			dl::RefCntAutoPtr<dl::IPipelineState> get_handle() { return pipeline_handle; }
			dl::RefCntAutoPtr<dl::IShaderResourceBinding> create_srb();
			void create() { Dev->CreateGraphicsPipelineState(m_ci, &pipeline_handle); }

		private:
			dl::GraphicsPipelineStateCreateInfo m_ci;
			dl::RefCntAutoPtr<dl::IPipelineState> pipeline_handle;
		};
	}
}