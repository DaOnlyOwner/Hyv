#include "rendering/render_systems.h"
#include "rendering/rendering_systems_ctxt.h"
#include "rendering/rendering_components.h"
#include "rendering/rendering.h"
#include "resource/resource_components.h"
#include "physics/physics_components.h"
#include "physics/physics_tools.h"
#include "MapHelper.hpp"


void hyv::rendering::create_geometry_pass_system(flecs::world& world)
{
	/*world.system<camera, >("CameraViewSetter").kind(flecs::OnUpdate).iter([](flecs::iter it, camera* cam) {
		auto thread_id = it.world().get_stage_id();
	auto& ctxt = DeferredCtxts[thread_id];
	{
		auto mapped = cam->constants.map(ctxt);
		mapped->eyePos = 
	}

		});*/
	
	// Transition states of global vbo and ibo on change
	world.observer<global_mesh_buffer>().event(flecs::OnSet).each([](global_mesh_buffer& gmb) {
		dl::StateTransitionDesc tdesc[2];
	tdesc[0] = dl::StateTransitionDesc(gmb.index_buffer,
		dl::RESOURCE_STATE_UNKNOWN,
		dl::RESOURCE_STATE_INDEX_BUFFER,
		dl::STATE_TRANSITION_FLAG_UPDATE_STATE);
	tdesc[1] = dl::StateTransitionDesc(gmb.vertex_buffer,
		dl::RESOURCE_STATE_UNKNOWN,
		dl::RESOURCE_STATE_VERTEX_BUFFER,
		dl::STATE_TRANSITION_FLAG_UPDATE_STATE);

	Imm->TransitionResourceStates(_countof(tdesc), tdesc);

		});

	//world.system<
	//	resource::static_mesh_gpu,
	//	physics::transform,
	//	resource::material>("GeometryRenderPassSystem")
	//		.multi_threaded()
	//		.kind(flecs::OnUpdate)
	//		.write(flecs::Wildcard)
	//		.iter([](
	//	flecs::iter it,
	//	resource::static_mesh_gpu* sm_ptr,
	//	physics::transform* trans_ptr,
	//	resource::material* mat_ptr) {
	//		auto thread_id = it.world().get_stage_id();
	//		auto& ctxt = DeferredCtxts[thread_id];
	//		//auto& consts = it.world().get_mut<geometry_pass_constants_vector>()[0][thread_id];
	//		auto global_mb = it.world().get_mut<global_mesh_buffer>();
	//		auto geom_pass_bundle = it.world().get_mut<geometry_pass_pipeline_bundle>();
	//		ctxt->Begin(0);

	//		dl::IBuffer* vbos[] = { global_mb->vertex_buffer.RawPtr() };
	//		ctxt->SetIndexBuffer(global_mb->index_buffer.RawPtr(), 0, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	//		auto camera_filter = it.world().filter_builder<camera, physics::transform>().with<MainCameraTag>().build();
	//		camera_filter.each([&](camera& cam, physics::transform& trans_cam) {
	//			dl::ITextureView* RTVs[] =
	//		{
	//			cam.albedo_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET),
	//			cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET)
	//		};

	//		dl::ITextureView* DSV = cam.depth_buffer->GetDefaultView(dl::TEXTURE_VIEW_DEPTH_STENCIL);

	//		ctxt->SetRenderTargets(_countof(RTVs), RTVs, DSV, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	//			
	//		for (auto i : it)
	//		{
	//			auto& sm = sm_ptr[i];
	//			auto& trans = trans_ptr[i];
	//			auto& mat = mat_ptr[i];
	//			auto [model, normal] = physics::get_model_normal(trans);
	//			
	//			{
	//				auto mapped = geom_pass_bundle->consts.map(ctxt.RawPtr());
	//				auto view = physics::get_view(trans_cam);
	//				auto MVP = glm::transpose(cam.projection * view * model);
	//				mapped->model = model;
	//				mapped->MVP = MVP;
	//				mapped->normal = normal;
	//			}

	//			u64 offset[] = { sm.offsetVertex * sizeof(vertex) };
	//			//HYV_INFO("{}", offset[0]);
	//			ctxt->SetVertexBuffers(0, 1, vbos, offset, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY, dl::SET_VERTEX_BUFFERS_FLAG_RESET);
	//			dl::DrawIndexedAttribs attribs;

	//			attribs.IndexType = dl::VT_UINT32;
	//			attribs.NumIndices = sm.numIndices;
	//			attribs.Flags = dl::DRAW_FLAG_VERIFY_ALL;
	//			attribs.FirstIndexLocation = sm.offsetIndex;

	//			ctxt->SetPipelineState(geom_pass_bundle->pso.get_handle());
	//			ctxt->CommitShaderResources(geom_pass_bundle->SRB, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	//			ctxt->DrawIndexed(attribs);
	//		}

	//		});
	//		dl::RefCntAutoPtr<dl::ICommandList> cmd_list;
	//		ctxt->FinishCommandList(&cmd_list);
	//		CmdLists[thread_id] = cmd_list;

	//		});

	world.system<>("GeometryPassSystem").iter([](flecs::iter it) {
		auto& geom = *it.world().get_mut<geometry_pass_pipeline_bundle>();
		auto& draws = *it.world().get_mut<indirect_draws_buffer>();
		auto& global_mb = *it.world().get_mut<global_mesh_buffer>();

		geom.pso.get_srb()->GetVariableByName(dl::SHADER_TYPE_VERTEX, "objs_data")->Set(geom.static_objects_data_buffer.get_buffer()->GetDefaultView(dl::BUFFER_VIEW_SHADER_RESOURCE));
		
		auto camera_filter = it.world().filter_builder<camera, physics::transform>().with<MainCameraTag>().build();
		camera_filter.each([&](camera& cam, physics::transform& trans_cam) {
			dl::ITextureView* RTVs[] =
		{
			cam.albedo_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET),
			cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET)
		};
		dl::ITextureView* DSV = cam.depth_buffer->GetDefaultView(dl::TEXTURE_VIEW_DEPTH_STENCIL);
		Imm->SetIndexBuffer(global_mb.index_buffer.RawPtr(), 0, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		
		u64 offset[] = { 0 };
		dl::IBuffer* vbos[] = { global_mb.vertex_buffer.RawPtr() };

		Imm->SetVertexBuffers(0, 1, vbos, offset, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY, dl::SET_VERTEX_BUFFERS_FLAG_RESET);

		Imm->SetRenderTargets(_countof(RTVs), RTVs, DSV, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		{
			auto m = geom.view.map();
			m->VP = glm::transpose(cam.projection * physics::get_view(trans_cam));
		}
		
		dl::DrawIndexedIndirectAttribs attribs;
		attribs.Flags = dl::DRAW_FLAG_VERIFY_ALL;
		attribs.pAttribsBuffer = draws.get_buffer();
		attribs.DrawCount = draws.get_size();
		attribs.DrawArgsStride = sizeof(draw_indirect_command);
		attribs.AttribsBufferStateTransitionMode = dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
		attribs.IndexType = dl::VT_UINT32;

		Imm->SetPipelineState(geom.pso.get_handle());
		Imm->CommitShaderResources(geom.pso.get_srb(), dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		Imm->DrawIndexedIndirect(attribs);

		});
		});



}

void hyv::rendering::create_composite_pass_system(flecs::world& world)
{

	//https://github.com/DiligentGraphics/DiligentSamples/blob/946e4393eee4db51fc7522098a814bbcfa47a978/Tutorials/Tutorial06_Multithreading/src/Tutorial06_Multithreading.cpp
	/*world.system<>("SubmitCommandBuffersSystem").iter([](flecs::iter it)
		{
			int countNotNull = 0;
	for (int i = 0; i < CmdLists.size(); i++)
	{
		if (CmdLists[i]) countNotNull++;
	}

	CmdPtrs.resize(countNotNull);
	for (int i = 0; i < CmdPtrs.size(); i++)
	{
		if (!CmdLists[i]) continue;
		CmdPtrs[i] = CmdLists[i].RawPtr();
	}

	Imm->ExecuteCommandLists(CmdPtrs.size(), CmdPtrs.data());
	for (auto& ctxt : DeferredCtxts)
	{
		ctxt->FinishFrame();
	}
	for (auto& l : CmdLists) l.Release();

		});*/

	world.system<camera>("CompositeRenderPassSystem").with<MainCameraTag>().each([](flecs::entity e, camera& cam)
		{
			auto* rtv = SwapChain->GetCurrentBackBufferRTV();
	auto* dsv = SwapChain->GetDepthBufferDSV();
	dl::StateTransitionDesc desc[] = {
		{cam.depth_buffer,dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_DEPTH_READ, dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{cam.albedo_buffer,dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_SHADER_RESOURCE, dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{cam.normal_buffer,dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_SHADER_RESOURCE, dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{rtv->GetTexture(),dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_RENDER_TARGET,dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{dsv->GetTexture(),dl::RESOURCE_STATE_UNKNOWN,dl::RESOURCE_STATE_DEPTH_WRITE,dl::STATE_TRANSITION_FLAG_UPDATE_STATE}
	};
	Imm->TransitionResourceStates(_countof(desc), desc);

	auto composite_pass = e.world().get_mut<composite_pass_pipeline_bundle>();

	Imm->SetRenderTargets(1, &rtv, dsv, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	Imm->ClearRenderTarget(rtv, cam.clear_color, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	Imm->SetPipelineState(composite_pass->pso.get_handle());
	Imm->CommitShaderResources(composite_pass->pso.get_srb(), dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	Imm->SetVertexBuffers(0, 0, nullptr, nullptr, dl::RESOURCE_STATE_TRANSITION_MODE_NONE, dl::SET_VERTEX_BUFFERS_FLAG_RESET);
	Imm->SetIndexBuffer(nullptr, 0, dl::RESOURCE_STATE_TRANSITION_MODE_NONE);
	Imm->Draw(dl::DrawAttribs{ 3,dl::DRAW_FLAG_VERIFY_ALL });

	dl::StateTransitionDesc desc_back[] = {
		{cam.depth_buffer, dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_DEPTH_WRITE, dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{cam.albedo_buffer,dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_RENDER_TARGET, dl::STATE_TRANSITION_FLAG_UPDATE_STATE},
		{cam.normal_buffer,dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_RENDER_TARGET, dl::STATE_TRANSITION_FLAG_UPDATE_STATE}
	};
	Imm->TransitionResourceStates(_countof(desc_back), desc_back);

	dl::ITextureView* cam_rtvs[] =
	{
		cam.albedo_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET),
		cam.normal_buffer->GetDefaultView(dl::TEXTURE_VIEW_RENDER_TARGET)
	};

	dl::ITextureView* cam_dsv = cam.depth_buffer->GetDefaultView(dl::TEXTURE_VIEW_DEPTH_STENCIL);

	Imm->SetRenderTargets(_countof(cam_rtvs), cam_rtvs, cam_dsv, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	for (int i = 0; i < _countof(cam_rtvs); i++)
	{
		Imm->ClearRenderTarget(cam_rtvs[i], cam.clear_color, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	}
	Imm->ClearDepthStencil(cam_dsv, dl::CLEAR_DEPTH_FLAG, 1.f, 0, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

	dl::ITextureView* rtvs[] = { rtv };
	Imm->SetRenderTargets(1, rtvs, dsv, dl::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		});
}
