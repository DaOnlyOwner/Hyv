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

#define DIFFUSE_FORMAT dl::TEX_FORMAT_RGBA8_UNORM;
#define NORMAL_FORMAT dl::TEX_FORMAT_RGBA16_FLOAT;
#define DEPTH_FORMAT dl::TEX_FORMAT_D32_FLOAT;

	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		//glm::vec3 tangent;
		//glm::vec3 bitangent;
		glm::vec2 uv;
	};

	struct init_info
	{
		enum class RenderBackend
		{
#if D3D12_SUPPORTED
			D3D12,
#endif

#if VULKAN_SUPPORTED
			Vulkan,
#endif
#if !D3D12_SUPPORTED && !VULKAN_SUPPORTED
			NoBackendSupported
#endif
		};
		bool enableDebugLayers = true;
		RenderBackend backend = RenderBackend::Vulkan;
		int width = 800, height = 600;
		std::string title;

	};

}