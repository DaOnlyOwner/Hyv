#include "resource/resource.h"
#include "resource/resource_components.h"
#include "logging.h"

hyv::resource::resource::resource(flecs::world& world) : world(world)
{
	
}

void hyv::resource::resource::set(const std::string& name, material& mat)
{
#ifndef NDEBUG

	if (name_to_material.contains(name)) HYV_WARN("Material with name {} already exists", name);
#endif

	name_to_material[name] = mat;
}

hyv::resource::material hyv::resource::resource::get_material(const std::string& name)
{
	auto it = name_to_material.find(name);
	if (it != name_to_material.end())
	{
		return it->second;
	}

	HYV_NON_FATAL_ERROR("No static mesh named {} in the resources", name);
	return material{};
}
