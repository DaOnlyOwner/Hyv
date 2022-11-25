#pragma once
#include "flecs.h"

namespace hyv
{
	namespace resource
	{
		class resource
		{
		public:
			resource(flecs::world& world);

		private:
			flecs::world& world;

		};
	}
}