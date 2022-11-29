#include "physics/physics_tools.h"


std::pair<glm::mat4, glm::mat4> hyv::physics::get_model_normal(const transform& trans)
{
	glm::mat4 m{ 1.0 };
	auto translationM = glm::translate(m, trans.position);
	auto rotM = glm::yawPitchRoll(trans.rotation.x, trans.rotation.y, trans.rotation.z);
	auto scaleM = glm::scale(m, trans.scale);
	auto model = (translationM * rotM * scaleM);
	auto normal = glm::inverse(glm::transpose(model));
	return { model,normal };
}