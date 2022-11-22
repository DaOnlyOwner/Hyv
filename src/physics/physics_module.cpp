#include "include/physics/physics_module.h"

physics::physics_module::physics_module(flecs::world& world)
{
	world.module<physics::physics_module>();
}
