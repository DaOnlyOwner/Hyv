#pragma once
#include "flecs.h"
#include <vector>
#include "resource_components.h"
#include "definitions.h"

class aiNode;
class aiScene;

namespace hyv
{
	namespace resource
	{
		struct static_mesh_loader_options
		{
			bool pretransform = false;
			bool merge = false;
			bool optimize = true;
			glm::vec3 size_factor = {1,1,1};
		};

		DEF_CUSTOM_EXCEPTION(no_texture_coords_error);
		DEF_CUSTOM_EXCEPTION(mesh_import_error);
		
		class loader
		{
		public:

			loader(flecs::world& world) : world(world){}

			/// <summary>
			/// Loads the specified static mesh into the world
			/// </summary>
			/// <param name="world">The world with the associated vertex buffer</param>
			/// <param name="file">The path to the static mesh</param>
			/// <param name="options">Loading options</param>
			void load_static_mesh(const char* file, static_mesh_loader_options options);
			
			
			~loader();

		private:

			void process_node_static_mesh(const aiNode& node, const aiScene& scene, static_mesh_loader_options options);

		private:
			flecs::world& world;
			std::vector<vertex> vertices;
			std::vector<u32> indices;
			

		};




	}
}
