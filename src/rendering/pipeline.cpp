#include "rendering/pipeline.h"


hyv::rendering::graphics_pipeline& hyv::rendering::graphics_pipeline::setup_geometry_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size)
{
    m_ci.GraphicsPipeline.NumRenderTargets = 2;
    m_ci.PSODesc.PipelineType = dl::PIPELINE_TYPE_GRAPHICS;
    m_ci.GraphicsPipeline.RTVFormats[0] = DIFFUSE_FORMAT;
    m_ci.GraphicsPipeline.RTVFormats[1] = NORMAL_FORMAT;
    m_ci.GraphicsPipeline.DSVFormat = DEPTH_FORMAT;
    m_ci.GraphicsPipeline.PrimitiveTopology = dl::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_ci.GraphicsPipeline.RasterizerDesc.CullMode = dl::CULL_MODE_BACK;
    m_ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    m_ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = true;
    m_ci.PSODesc.ResourceLayout.DefaultVariableType = dl::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    m_ci.pVS = vs.get_shader_handle();
    m_ci.pPS = ps.get_shader_handle();
    if (descs != nullptr)
    {
        m_ci.PSODesc.ResourceLayout.ImmutableSamplers = descs;
        m_ci.PSODesc.ResourceLayout.NumImmutableSamplers = descs_size;
    }

    dl::LayoutElement elems[] = {
        dl::LayoutElement{0, 0, 3, dl::VT_FLOAT32, dl::False},
        dl::LayoutElement(1, 0, 3, dl::VT_FLOAT32, dl::False),
        dl::LayoutElement(2, 0, 2, dl::VT_FLOAT32, dl::False),
    };

    m_ci.GraphicsPipeline.InputLayout.LayoutElements = elems;
    m_ci.GraphicsPipeline.InputLayout.NumElements = _countof(elems);
    m_ci.PSODesc.Name = "Geometry Pass Pipeline";

    create();
    return *this;
}

hyv::rendering::graphics_pipeline& hyv::rendering::graphics_pipeline::setup_composite_pass(shader& vs, shader& ps, dl::ImmutableSamplerDesc* descs, int descs_size)
{
    m_ci.PSODesc.Name = "Composite PSO";
    m_ci.PSODesc.PipelineType = dl::PIPELINE_TYPE_GRAPHICS;
    m_ci.PSODesc.ResourceLayout.DefaultVariableType = dl::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;
    m_ci.GraphicsPipeline.PrimitiveTopology = dl::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_ci.GraphicsPipeline.NumRenderTargets = 1;
    m_ci.GraphicsPipeline.RTVFormats[0] = SwapChain->GetDesc().ColorBufferFormat;
    m_ci.GraphicsPipeline.DSVFormat = SwapChain->GetDesc().DepthBufferFormat;
    m_ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;
    m_ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    m_ci.GraphicsPipeline.RasterizerDesc.CullMode = dl::CULL_MODE_NONE;
    m_ci.pVS = vs.get_shader_handle();
    m_ci.pPS = ps.get_shader_handle();
    if (descs != nullptr)
    {
        m_ci.PSODesc.ResourceLayout.ImmutableSamplers = descs;
        m_ci.PSODesc.ResourceLayout.NumImmutableSamplers = descs_size;
    }
    m_ci.PSODesc.Name = "Composite Pass Pipeline";

    create();
    return *this;
}

dl::RefCntAutoPtr<dl::IShaderResourceBinding> hyv::rendering::graphics_pipeline::create_srb() {
    dl::RefCntAutoPtr<dl::IShaderResourceBinding> binding;
    pipeline_handle->CreateShaderResourceBinding(&binding);
    return binding;
}
