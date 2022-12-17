#include "rendering/rendering_module.h"
#include "rendering/render_systems.h"
#include "rendering/rendering_components.h"
#include "rendering/rendering.h"
#include "rendering/buffer.h"
#include "rendering/rendering.h"
#include "windowing/windowing.h"
#include "global.h"
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

void hyv::rendering::rendering_module::observe_main_camera(flecs::world& world)
{
    world.observer<MainCameraTag>().event(flecs::OnSet).each([&](flecs::entity e, MainCameraTag tag) {

        int camera_count = 0;
    world.filter<camera, MainCameraTag>().each([&](camera& cam, MainCameraTag tag2) {
        camera_count++;
    if (camera_count > 1)
    {
        HYV_NON_FATAL_ERROR("Cameras with tag MainCameraTag > 1")
            return;
    }
    auto bundle = e.world().get_mut<composite_pass_pipeline_bundle>();
    bundle->pso.get_srb()->GetVariableByName(dl::SHADER_TYPE_PIXEL, "GBuffer_Normal")
        ->Set(cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_SHADER_RESOURCE));
        });
        });
}



hyv::rendering::rendering_module::rendering_module(flecs::world& world)
{
    world.module<rendering_module>("RendererModule");
    const auto& info = *world.get_mut<init_info>();
    
    auto& win = windowing::windowing::inst(info);
    auto& ren = rendering::inst(info, win);


    observe_and_init_cameras(world);
    observe_main_camera(world);
    create_geometry_pass_system(world);
    create_composite_pass_system(world);
    world.system<>("ImGuiRenderPassSystem").iter([](flecs::iter it) {
        rendering::inst().render_imgui();
        });


    init_composite_pass(world);
}

void hyv::rendering::rendering_module::init_composite_pass(flecs::world& world)
{
    composite_pass_pipeline_bundle pbundle;
    shader cvs(shader_type::Vertex, SHADER_RES "/rendering/composite_vs.hlsl", "Composite Pass Vertex Shader");
    shader cps(shader_type::Pixel, SHADER_RES "/rendering/composite_ps.hlsl", "Composite Pass Pixel Shader");
    cvs.ok();
    cps.ok();
    pbundle.pso.setup_composite_pass(cvs, cps, nullptr, 0);
    pbundle.pso.init_srb();
    world.set<composite_pass_pipeline_bundle>(std::move(pbundle));
}

void hyv::rendering::rendering_module::observe_and_init_cameras(flecs::world& world)
{
    world.observer<camera>().event(flecs::OnSet).each([](flecs::entity e, camera& cam) {
        init_camera(cam);
        });
}
