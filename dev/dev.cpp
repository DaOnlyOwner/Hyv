#include "flecs.h"
#include "resource/loader.h"
#include "resource/resource_components.h"
#include "resource/resource.h"	
#include "rendering/rendering.h"
#include "rendering/rendering_module.h"
#include "rendering/rendering_components.h"
#include "rendering/static_scene_guard.h"
#include "physics/physics_components.h"
#include "physics/physics_module.h"
#include "physics/physics_tools.h"
#include "windowing/windowing.h"
#include "definitions.h"
#include "logging.h"


#define SPEED 5.f
#define ROTATION_SPEED 5.f


using namespace hyv;

int main()
{
	flecs::world world;
	
	hyv::init_info info;
#ifndef NDEBUG
	info.enableDebugLayers = true;
#else
	info.enableDebugLayers = false;
#endif
	//info.backend = hyv::init_info::RenderBackend::D3D12;

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

		{
			rendering::static_scene_guard guard(world);
			for (int i = 0; i < 100; i++)
			{
				for (auto& bundle : bundles)
				{
					world.entity()
						.set<hyv::resource::static_mesh_gpu>(bundle.gpu_)
						.set<hyv::physics::transform>({})
						.set<hyv::resource::material>({});
				}
			}
		}
	}
	hyv::rendering::camera cam;
	cam.projection = glm::perspectiveLH(80.f, info.width / (float)info.height, 0.1f, 10000.f);
	cam.width = info.width;
	cam.height = info.height;
	cam.clear_color[0] = 0;
	cam.clear_color[1] = 0;
	cam.clear_color[2] = 0;
	cam.clear_color[3] = 0;	
	hyv::physics::transform cam_trans{ { 1,3,0 },{ 0,0,0 },{ 1,1,1 } };

	world.entity("Camera").set<hyv::rendering::camera>(cam)
		.set<hyv::physics::transform>(cam_trans)
		.add<hyv::rendering::MainCameraTag>();
	
	//world.set_threads(ren.num_threads());
	//world.set_target_fps(60);
	
	glm::vec2 mousePos = win.get_mouse_pos();
	world.system<hyv::rendering::camera, physics::transform>().with<rendering::MainCameraTag>().ctx(&mousePos).iter([&](flecs::iter it, rendering::camera* cam, physics::transform* trans) {
		for (auto i : it)
		{
			auto& t = trans[i];
			auto& old = ((glm::vec2*)it.ctx())[0];
			glm::vec3 delta;
			auto window = win.get_window();
			float deltaTime = it.delta_time() / 1000.f;

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) physics::move(t,WORLD_UP * SPEED * deltaTime);
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) physics::move(t,WORLD_UP * -SPEED * deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) physics::move(t,WORLD_RGT * -SPEED * deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) physics::move(t,WORLD_RGT * SPEED * deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) physics::move(t,WORLD_FWD *-SPEED * deltaTime);
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) physics::move(t,WORLD_FWD * SPEED * deltaTime);

			auto mouse_pos_now = win.get_mouse_pos();
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
				auto delta = old - mouse_pos_now;
				physics::rotate(t,glm::vec3(delta.x,delta.y, 0) * ROTATION_SPEED * deltaTime);
			}
			old = mouse_pos_now;

		}
		});
		
	double average = 0;
	long long int count = 0;
	while (win.should_stay_up())
	{
		if (count % 1000 == 0)
		{
			HYV_INFO("Average over 1000 frames: {}", average / 1000.f);
			count = 0;
			average = 0;
		}
		win.new_frame();
		ren.new_frame(cam.clear_color);
		world.progress(win.delta_time());
		ren.end_frame();
		win.end_frame();
		count++;
		average+=win.delta_time();
	}
	return 0;	
}