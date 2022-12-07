#pragma once
#include "flecs.h"

namespace hyv
{
	namespace rendering
	{
		class static_scene_guard
		{
		public:
			static_scene_guard(flecs::world& world) : m_world(world){}
			~static_scene_guard();

		private:
			flecs::world& m_world;

		};
	}
}
