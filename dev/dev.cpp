#include "flecs.h"
#include "resource/loader.h"
#include "resource/resource_components.h"
#include "resource/resource.h"	
#include "rendering/rendering.h"
#include "rendering/rendering_module.h"
#include "rendering/rendering_components.h"
#include "physics/physics_components.h"
#include "physics/physics_module.h"
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

	world.import<flecs::monitor>();
	world.import<hyv::rendering::rendering_module>();
	world.import<hyv::physics::physics_module>();

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
				.set<hyv::physics::transform>({})
				.set<hyv::resource::material>({});
		}

	}


	hyv::rendering::main_camera cam;
	cam.cam.projection = glm::perspective(80.f, info.width / (float)info.height, 0.1f, 10000.f);
	cam.cam.width = info.width;
	cam.cam.height = info.height;
	cam.cam.clear_color[0] = 0;
	cam.cam.clear_color[1] = 0;
	cam.cam.clear_color[2] = 0;
	cam.cam.clear_color[3] = 0;	
	hyv::physics::transform cam_trans{ { 0,5,5 },{ 0,0,0 },{ 1,1,1 } };

	world.set<hyv::rendering::main_camera>(cam);
	world.set<hyv::physics::main_camera_transform>({ cam_trans });
	
	world.set_threads(ren.num_threads());
	//world.set_target_fps(60);
	
	while (win.should_stay_up())
	{
		win.new_frame();
		ren.new_frame(cam.cam.clear_color);
		world.progress(win.delta_time());
		ren.end_frame();
		win.end_frame();
	}
	return 0;
}