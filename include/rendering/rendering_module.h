#pragma once

#include "flecs.h"

namespace hyv
{
	namespace rendering
	{
		struct static_mesh_renderer_module
		{
		public:
			static_mesh_renderer_module(flecs::world& world);
		};
	}
}