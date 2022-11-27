#include "rendering/rendering_module.h"
#include "rendering/render_systems.h"
#include "resource/resource_components.h"
#include "global.h"
#include "rendering/buffer.h"
#include <string>

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
	}
}


hyv::rendering::static_mesh_renderer_module::static_mesh_renderer_module(flecs::world& world)
{
	world.module<static_mesh_renderer_module>("StaticMeshRendererModule");
	create_geometry_pass_system(world);
	world.observer<camera>().event(flecs::OnSet).each([](flecs::entity& e, camera& cam) {
        init_camera(cam);
		});
    geometry_pass_constants_vector v;
    v.reserve(DeferredCtxts.size());
    for (int i = 0; i < DeferredCtxts.size(); i++)
    {
        v.push_back(uniform_buffer<geometry_pass_VS_per_mesh_constants>(std::string("Geometry Pass Per Mesh Constants ") + std::to_string(i)));
    }

    world.set<geometry_pass_constants_vector>(std::move(v));
}
