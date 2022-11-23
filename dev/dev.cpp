#include "flecs.h"
#include "resource/loader.h"

int main()
{
	flecs::world world;

	{
		hyv::resource::loader loader(world);
		hyv::resource::static_mesh_loader_options options;
		options.merge = true;
		loader.load_static_mesh("sponza.fbx", options);
	}

}