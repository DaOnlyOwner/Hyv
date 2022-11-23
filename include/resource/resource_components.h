#pragma once
#include <memory>
#include "definitions.h"
#include "DynamicBuffer.hpp"
#include <unordered_map>
#include <string>
#include "global.h"

namespace hyv
{
	namespace resource
	{

		struct static_mesh
		{
			u32 numIndices = 0;
			u32 numVertices = 0;
			u64 offsetIndex = 0;
			u64 offsetVertex = 0;
		};

		struct material
		{

		};

		struct mesh_buffer
		{
			dl::RefCntAutoPtr<Diligent::IBuffer> vertex_buffer;
			dl::RefCntAutoPtr<Diligent::IBuffer> index_buffer;
			std::unordered_map<std::string, static_mesh> name_to_mesh;
		};
	}
}