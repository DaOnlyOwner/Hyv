#pragma once

#include <cstdint>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include <string>

#define DEF_CUSTOM_EXCEPTION(name) \
class name : public std::exception\
{\
std::string msg;\
public:\
	name(const std::string& msg):msg(msg){}\
	const char* what()\
	{\
		return msg.c_str();\
	}\
}

namespace hyv
{
	typedef uint32_t u32;
	typedef uint64_t u64;
	typedef int32_t i32;
	typedef int64_t i64;

	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		//glm::vec3 tangent;
		//glm::vec3 bitangent;
		glm::vec2 uv;
	};

}