#include "physics/physics_tools.h"

std::pair<glm::mat4, glm::mat4> hyv::physics::get_model_normal(const position& pos, const rotation& rot, const scale& s)
{
	glm::mat4 m{ 1.0 };
	auto translationM = glm::translate(m, pos);
	auto rotM = glm::yawPitchRoll(rot.x, rot.y, rot.z);
	auto scaleM = glm::scale(m, s);
	auto model = (translationM * rotM * scaleM);
	auto normal = glm::inverse(glm::transpose(model));
	return { model,normal };
}