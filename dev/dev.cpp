#include "flecs.h"
#include "resource/loader.h"
#include "resource/resource_components.h"
#include "resource/resource.h"	
#include "rendering/rendering.h"
#include "rendering/rendering_module.h"
#include "rendering/rendering_components.h"
#include "physics/physics_components.h"
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
		hyv::resource::asset_loader loader(res);
		hyv::resource::static_mesh_loader_options options;
		options.merge = false;
		options.pretransform = true;
		auto bundles = loader.load_static_mesh(RES"/models/sponza/sponza.obj", options);

		for (auto& bundle : bundles)
		{
			world.entity()
				.set<hyv::resource::static_mesh_gpu>(bundle.gpu_)
				.set<hyv::physics::position>({})
				.set<hyv::physics::rotation>({})
				.set<hyv::physics::scale>({ 1,1,1 });
		}

	}

	hyv::rendering::camera cam;
	cam.projection = glm::perspective(80.f, info.width / (float)info.height, 0.1f, 10000.f);
	hyv::physics::position cam_pos = { 0,5,5 };
	hyv::physics::rotation cam_rot = { 0,0,0 };
	hyv::physics::scale cam_scale = { 1,1,1 };

	world.entity()
		.set<hyv::rendering::camera>(cam)
		.set<hyv::physics::position>(cam_pos)
		.set<hyv::physics::rotation>(cam_rot)
		.set<hyv::physics::scale>(cam_scale);
	

	world.import<flecs::monitor>();
	world.import<hyv::rendering::rendering_module>();
	
	

	world.set_threads(ren.num_threads());
	
	while (win.should_stay_up())
	{

		win.new_frame();
		world.progress(win.delta_time());
		win.end_frame();

	}
	return 0;
}