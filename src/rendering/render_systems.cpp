#include "rendering/render_systems.h"
#include "resource/resource_components.h"
#include "logging.h"
#include "rendering/rendering_components.h"
#include "physics/physics_components.h"
#include "physics/physics_tools.h"
#include "MapHelper.hpp"
#include "rendering/buffer.h"

void hyv::rendering::create_mesh_renderer_system(flecs::world& world)
{
	world.system<resource::static_mesh_gpu, physics::position, physics::rotation, physics::scale>("MeshRenderSystem").multi_threaded().kind(flecs::OnUpdate).iter([&world](flecs::iter it,
		resource::static_mesh_gpu* sm_ptr,
		physics::position* pos_ptr,
		physics::rotation* rot_ptr,
		physics::scale* scale_ptr) {
			auto thread_id = it.world().get_stage_id();
			auto& ctxt = DeferredCtxts[thread_id];
			auto& cmd_list = CmdLists[thread_id];
			auto& consts = world.get_mut<geometry_pass_constants_vector>()[0][thread_id];

			for (auto i : it)
			{
				auto& sm = sm_ptr[i];
				auto& pos = pos_ptr[i];
				auto& scale = scale_ptr[i];
				auto& rot = rot_ptr[i];
				auto [model, normal] = physics::get_model_normal(pos, rot, scale);
				world.query<camera, physics::position, physics::rotation>().each([&](flecs::entity& e, camera& cam, const physics::position& cam_pos, const physics::rotation& cam_rot) {
					
					dl::ITextureView* RTVs[] =
					{
						cam.albedo_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET),
						cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET)
					};

					dl::ITextureView* DSV = cam.depth_buffer->GetDefaultView(dl::TEXTURE_VIEW_DEPTH_STENCIL);

					ctxt->SetRenderTargets(_countof(RTVs), RTVs, DSV, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

					{
						auto mapped = consts.map(ctxt.RawPtr());
						auto [view, _2] = physics::get_model_normal(cam_pos, { 1,1,1 }, cam_rot);
						auto MVP = cam.projection * view * model;
						mapped->model = model;
						mapped->MVP = MVP;
						mapped->normal = normal;
					}




					});
			}
			});
	
}
