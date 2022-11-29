#include "rendering/rendering_module.h"
#include "rendering/render_systems.h"
#include "rendering/rendering_components.h"
#include "global.h"
#include "rendering/buffer.h"
#include <string>
#include "logging.h"

namespace
{
    void init_camera(hyv::rendering::camera& cam)
    {
        const auto& SCDesc = hyv::SwapChain->GetDesc();

        dl::TextureDesc desc;
        desc.Name = "Albedo GBuffer";
        desc.Type = dl::RESOURCE_DIM_TEX_2D;
        desc.BindFlags = dl::BIND_RENDER_TARGET | dl::BIND_SHADER_RESOURCE;
        desc.Format = DIFFUSE_FORMAT;
        desc.Width = cam.width;
        desc.Height = cam.height;
        cam.albedo_buffer.Release();
        hyv::Dev->CreateTexture(desc, nullptr, &cam.albedo_buffer);

        desc.Name = "Normal GBuffer";
        desc.Format = NORMAL_FORMAT;
        desc.BindFlags = dl::BIND_RENDER_TARGET | dl::BIND_SHADER_RESOURCE;
        cam.normal_buffer.Release();
        hyv::Dev->CreateTexture(desc, nullptr, &cam.normal_buffer);

        //desc.Name = "Position GBuffer";
        //gbuffer.positionBuffer.Release();
        //device->CreateTexture(desc, nullptr, &gbuffer.positionBuffer);

        desc.Name = "Depth GBuffer";
        desc.BindFlags = dl::BIND_DEPTH_STENCIL | dl::BIND_SHADER_RESOURCE;
        desc.Format = DEPTH_FORMAT;
        cam.depth_buffer.Release();
        hyv::Dev->CreateTexture(desc, nullptr, &cam.depth_buffer);
        dl::StateTransitionDesc descs[3] = {
            {cam.depth_buffer,dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_DEPTH_WRITE,dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
            {cam.albedo_buffer, dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_RENDER_TARGET, dl::STATE_TRANSITION_FLAG_UPDATE_STATE },
            {cam.normal_buffer, dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_RENDER_TARGET, dl::STATE_TRANSITION_FLAG_UPDATE_STATE}
        };

        hyv::Imm->TransitionResourceStates(_countof(descs), descs);

    }
}


hyv::rendering::rendering_module::rendering_module(flecs::world& world)
{
	world.module<rendering_module>("StaticMeshRendererModule");
	world.observer<camera>().event(flecs::OnSet).each([](flecs::entity e, camera& cam) {
        init_camera(cam);
		});
    world.observer<main_camera>().event(flecs::OnSet).each([](flecs::entity e, main_camera& cam) {
        init_camera(cam.cam);
        auto bundle = e.world().get_mut<composite_pass_pipeline_bundle>();
        bundle->SRB->GetVariableByName(dl::SHADER_TYPE_PIXEL, "GBuffer_Normal")
        ->Set(cam.cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_SHADER_RESOURCE));
        });



	create_geometry_pass_system(world);
    create_composite_pass_system(world);
    
    /*geometry_pass_constants_vector v;
    v.reserve(DeferredCtxts.size());
    for (int i = 0; i < DeferredCtxts.size(); i++)
    {
        v.push_back(uniform_buffer<geometry_pass_constants>(std::string("Geometry Pass Per Mesh Constants ") + std::to_string(i)));
    }*/


    geometry_pass_pipeline_bundle gbundle;
    shader gvs(shader_type::Vertex, SHADER_RES "/rendering/geometry_vs.hlsl", "Geometry Pass Vertex Shader");
    shader gps(shader_type::Pixel, SHADER_RES "/rendering/geometry_ps.hlsl", "Geometry Pass Pixel Shader");
    gvs.ok();
    gps.ok();
    gbundle.pso.setup_geometry_pass(gvs, gps, nullptr, 0);
    gbundle.SRB = gbundle.pso.create_srb();
    gbundle.consts = uniform_buffer<geometry_pass_constants>("Geometric Constants");
    gbundle.pso.get_handle()->GetStaticVariableByName(dl::SHADER_TYPE_VERTEX, "mesh_consts")
        ->Set(gbundle.consts.get_buffer());
    gbundle.pso.get_handle()->InitializeStaticSRBResources(gbundle.SRB);

    composite_pass_pipeline_bundle pbundle;
    shader cvs(shader_type::Vertex, SHADER_RES "/rendering/composite_vs.hlsl", "Composite Pass Vertex Shader");
    shader cps(shader_type::Pixel, SHADER_RES "/rendering/composite_ps.hlsl", "Composite Pass Pixel Shader");
    cvs.ok();
    cps.ok();
    pbundle.pso.setup_composite_pass(cvs, cps, nullptr, 0);
    pbundle.SRB = pbundle.pso.create_srb();

    // Set singletons
    world.set<geometry_pass_pipeline_bundle>(std::move(gbundle));
    world.set<composite_pass_pipeline_bundle>(std::move(pbundle));
}
