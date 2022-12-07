#include "rendering/static_scene_guard.h"
#include "rendering/rendering_components.h"
#include "physics/physics_components.h"
#include "resource/resource_components.h"
#include "physics/physics_tools.h"

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
	
	object_data_buffer objs_static("Object Data Static", model_matrices.data(),dl::BIND_SHADER_RESOURCE);
	indirect_draws_buffer draw_commands_static("Indirect Draw Commands Static", dics.data(), dl::BIND_INDIRECT_DRAW_ARGS);
	
	m_world.set<object_data_buffer>(objs_static);
	m_world.set<indirect_draws_buffer>(draw_commands_static);
}
