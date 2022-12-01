#include "physics/physics_module.h"
#include "physics/physics_components.h"

hyv::physics::physics_module::physics_module(flecs::world& world)
{
	world.module<hyv::physics::physics_module>();

	world.component<glm::vec3>()
		.member<float>("x")
		.member<float>("y")
		.member<float>("z");

	world.component<glm::vec2>()
		.member<float>("theta")
		.member<float>("phi");

	world.component<transform>()
		.member<glm::vec3>("position")
		.member<glm::vec2>("rotation")
		.member<glm::vec3>("scale");
}
