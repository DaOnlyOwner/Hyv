#pragma once

#include "flecs.h"
#include "include/physics/physics_components.h"
#include "include/rendering/rendering_components.h"



namespace rendering
{

	/// <summary>
	/// Creates a system that renders all mesh entities to all cameras.
	/// </summary>
	/// <param name="world">The world of which the entities should be rendered</param>
	void create_mesh_renderer_system(flecs::world& world);

	/// <summary>
	/// Creates a system that performs gbuffer shading on the cameras.
	/// </summary>
	/// <param name="world"></param>
	void create_gbuffer_shading_system(flecs::world& world);
}