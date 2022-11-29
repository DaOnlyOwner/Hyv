#include "windowing/windowing.h"

hyv::windowing::windowing::windowing(const init_info& info)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_glfw_window = glfwCreateWindow(info.width, info.height, info.title.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(m_glfw_window, this);
}

bool hyv::windowing::windowing::should_stay_up()
{
	return !glfwWindowShouldClose(m_glfw_window);
}

void hyv::windowing::windowing::new_frame()
{
	m_start_of_frame = clock::now();
	glfwPollEvents();
}

void hyv::windowing::windowing::end_frame()
{
	glfwSwapBuffers(m_glfw_window);
	auto end_of_frame = clock::now();
	m_delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_of_frame - m_start_of_frame).count() / 1000.f / 1000.f;
}

float hyv::windowing::windowing::delta_time()
{
	return m_delta_time;
}

hyv::windowing::windowing::~windowing()
{
	glfwTerminate();
}

hyv::windowing::windowing& hyv::windowing::windowing::inst(const init_info& info)
{
	static windowing instance(info);
	return instance;
}

hyv::windowing::windowing& hyv::windowing::windowing::inst()
{
	return inst({});
}
