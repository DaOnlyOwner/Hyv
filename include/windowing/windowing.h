#pragma once

#include "GLFW/glfw3.h"
#include <memory>
#include "definitions.h"
#include <chrono>
#include <type_traits>


namespace hyv
{
	namespace windowing
	{
		class windowing
		{
		public:
			bool should_stay_up();
			void new_frame();
			void end_frame();
			float delta_time();
			~windowing();
			GLFWwindow* get_window() { return m_glfw_window; }
			static windowing& inst(const init_info&); 
			static windowing& inst();

		private:
			//https://stackoverflow.com/questions/49090366/how-to-convert-stdchronohigh-resolution-clocknow-to-milliseconds-micros
			using clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
				std::chrono::high_resolution_clock,
				std::chrono::steady_clock>;

			GLFWwindow* m_glfw_window;
			float m_delta_time = 0.0000001f;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_start_of_frame;
			windowing(const init_info&);

		};
	}
}