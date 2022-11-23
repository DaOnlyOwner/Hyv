#pragma once

#include "flecs.h"

namespace hyv
{
	namespace rendering
	{
		struct rendering_module
		{
		public:
			rendering_module(flecs::world& world);
		};
	}
}