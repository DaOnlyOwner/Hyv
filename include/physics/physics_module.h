#pragma once

#include "flecs.h"

namespace hyv
{
	namespace physics
	{
		struct physics_module
		{
		public:
			physics_module(flecs::world& world);
		};
	}
}