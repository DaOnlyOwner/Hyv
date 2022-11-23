#include "physics/physics_module.h"

hyv::physics::physics_module::physics_module(flecs::world& world)
{
	world.module<hyv::physics::physics_module>();
}
