#pragma once
#include "flecs.h"
#include <unordered_map>
#include "resource/resource_components.h"
#include <tuple>
#include "logging.h"
#include <type_traits>


namespace hyv
{
	namespace resource
	{

		struct static_mesh_bundle
		{
			static_mesh_cpu cpu_;
			static_mesh_gpu gpu_;
		};

		class resource
		{
		public:
			resource(flecs::world& world);
			flecs::world& get_world() { return world; }
			friend class asset_loader;

			template<typename T>
			T get(const std::string& name)
			{
				static_assert(std::is_same_v<T, static_mesh_cpu> || std::is_same_v<T, static_mesh_gpu>, "The type is not supported by the resource class");
				if constexpr (std::is_same<T, static_mesh_cpu>::value || std::is_same<T, static_mesh_gpu>::value)
				{
					return get_static_mesh<T>(name);
				}

				return T{};

			}

		private:
			template<typename T>
			T get_static_mesh(const std::string& name)
			{
				auto it = name_to_static_mesh.find(name);
				if (it != name_to_static_mesh.end())
				{
					if constexpr (std::is_same<T, static_mesh_cpu>::value)
						return it->second.cpu_;
					else return it->second.gpu_;
				}

				HYV_NON_FATAL_ERROR("No static mesh named {} in the resources", name);
				return T{};
			}
			std::unordered_map<std::string, static_mesh_bundle> name_to_static_mesh;
			//std::unordered_map<std::string, std::string> name_to_file;
			flecs::world& world;

		};
	}
}