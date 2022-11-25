#include "resource/resource.h"
#include "resource/resource_components.h"
#include "logging.h"

hyv::resource::resource::resource(flecs::world& world) : world(world)
{
	world.set<mesh_buffer>({});
	world.observer<mesh_buffer>().event(flecs::OnSet).each([&world](mesh_buffer& mbuffer) {
		HYV_INFO("mesh_buffer was (re)build");
		world.query<static_mesh, static_mesh_name>().each([&mbuffer](static_mesh& smesh, const static_mesh_name& name) {
			auto it = mbuffer.name_to_mesh.find(name);
			if (it != mbuffer.name_to_mesh.end())
			{
				smesh = it->second;
			}

			else
			{
				HYV_WARN("Static Mesh {} is unloaded. This will result in visual errors.", name);
			}

			});
		});
}
