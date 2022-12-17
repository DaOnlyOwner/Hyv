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

			void init_composite_pass(flecs::world& world);

		private:
			void observe_and_init_cameras(flecs::world& world);
			void observe_main_camera(flecs::world& world);
		};
	}
}