#include "rendering/static_scene_guard.h"
#include "rendering/rendering_components.h"
#include "physics/physics_components.h"
#include "resource/resource_components.h"
#include "physics/physics_tools.h"


hyv::rendering::graphics_pipeline hyv::rendering::static_scene_guard::init_static_geometry_pso()
{

	graphics_pipeline pso;
	shader gvs(shader_type::Vertex, SHADER_RES "/rendering/geometry_vs.hlsl", "Geometry Pass Vertex Shader");
	shader gps(shader_type::Pixel, SHADER_RES "/rendering/geometry_ps.hlsl", "Geometry Pass Pixel Shader");
	gvs.ok();
	gps.ok();
	pso.setup_geometry_pass(gvs, gps, nullptr, 0);
	return pso;
}

hyv::rendering::static_scene_guard::~static_scene_guard()
{
	std::vector<model_normal_bundle> model_matrices;
	std::vector<draw_indirect_command> dics;
	int count = 0;

	m_world.query<const resource::static_mesh_gpu, const physics::transform>()
		.each([&](flecs::entity e, const resource::static_mesh_gpu& sm, const physics::transform& trans)
			{
				auto [model, normal] = physics::get_model_normal(trans);
				auto m_n = model_normal_bundle(model, normal);
				model_matrices.push_back(m_n);

				draw_indirect_command cmd;
				cmd.base_vertex = sm.offsetVertex;
				cmd.first_index_location = sm.offsetIndex;
				cmd.num_indices = sm.numIndices;
				cmd.num_instances = 1;
				cmd.first_instance_location = count++;
				
				dics.push_back(cmd);
			});
	
	struct_buffer<model_normal_bundle> objs_static("Object Data Static", model_matrices.data(), model_matrices.size(), dl::BIND_SHADER_RESOURCE);
	indirect_draws_buffer draw_commands_static("Indirect Draw Commands Static", dics.data(), dics.size(), dl::BIND_INDIRECT_DRAW_ARGS);
	
	geometry_pass_pipeline_bundle gbundle;

	auto pso = init_static_geometry_pso();
	gbundle.pso = pso;
	gbundle.static_objects_data_buffer = objs_static;
	gbundle.view = uniform_buffer<geometry_pass_pipeline_bundle::VPMatrix>("VPMatrix");
	gbundle.pso.get_srb()->GetVariableByName(dl::SHADER_TYPE_VERTEX, "VP")->Set(gbundle.view.get_buffer());
	gbundle.pso.init_srb();

	m_world.set<geometry_pass_pipeline_bundle>(gbundle);
	m_world.set<indirect_draws_buffer>(draw_commands_static);
}
