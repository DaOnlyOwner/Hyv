#pragma once

#include "flecs.h"

namespace rendering
{
	struct rendering_module
	{
	public:
		rendering_module(flecs::world& world);
	};
}