#pragma once
#include "resource_components.h"
#include "flecs.h"


namespace hyv
{
	namespace resource
	{
		/// <summary>
		/// Returns the mesh associated with this name
		/// </summary>
		/// <param name="world">The world the mesh was loaded in</param>
		/// <param name="name">The name of the mesh</param>
		/// <returns>The associated mesh</returns>
		static_mesh get_static_mesh(flecs::world& world, const char* name);
	}
}