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
			graphics_pipeline() {}
			graphics_pipeline& setup_geometry_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size);
			graphics_pipeline& setup_composite_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size);
			dl::RefCntAutoPtr<dl::IPipelineState> get_handle() { return pipeline_handle; }
			dl::IShaderResourceBinding* get_srb() { return m_srb.RawPtr(); }
			void create_pso() { Dev->CreateGraphicsPipelineState(m_ci, &pipeline_handle); pipeline_handle->CreateShaderResourceBinding(&m_srb);}
			void init_srb() { pipeline_handle->InitializeStaticSRBResources(m_srb); }

		private:
			dl::GraphicsPipelineStateCreateInfo m_ci;
			dl::RefCntAutoPtr<dl::IPipelineState> pipeline_handle;
			dl::RefCntAutoPtr<dl::IShaderResourceBinding> m_srb;
		};
	}
}