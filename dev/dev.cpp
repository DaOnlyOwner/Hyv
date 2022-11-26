#include "flecs.h"
#include "resource/loader.h"
#include "resource/resource_components.h"
#include "resource/resource.h"	
#include "rendering/rendering.h"
#include "rendering/rendering_module.h"
#include "windowing/windowing.h"
#include "definitions.h"
#include "logging.h"


struct A
{
	int a = 0;
	int b = 5;
};

int main()
{
	flecs::world world;
	
	hyv::init_info info;

	hyv::resource::resource res(world);
	auto& win = hyv::windowing::windowing::inst(info);
	auto& ren = hyv::rendering::rendering::inst(info, win);

	world.set<flecs::Rest>({});
	{
		hyv::resource::asset_loader loader(world);
		hyv::resource::static_mesh_loader_options options;
		options.merge = true;
		loader.load_static_mesh(RES"/models/sponza/sponza.obj", options);
	}
	world.import<flecs::monitor>();
	world.import<hyv::rendering::static_mesh_renderer_module>();
	world.component<A>("TestA");
	
	for (int i = 0; i < 100000; i++)
	{
		world.entity().set<hyv::resource::static_mesh>({});
	}

	world.set_threads(5);
	
	while (win.should_stay_up())
	{

		win.new_frame();
		world.progress(win.delta_time());
		win.end_frame();

	}
	return 0;

}