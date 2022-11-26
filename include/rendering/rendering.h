#pragma once
#include <string>
#include "global.h"
#include "definitions.h"

#include "imgui.h"
#include "ImGuiImplDiligent.hpp"
#include "ImGuiUtils.hpp"
#include "windowing/windowing.h"
#include "flecs.h"

namespace hyv
{
	namespace rendering
	{

		class rendering
		{
		public:
			void new_frame();
			void end_frame();
			~rendering();
			float clear_color[4];
			static rendering& inst(const init_info& info, windowing::windowing& w);
			static rendering& inst();

			int num_threads() { return DeferredCtxts.size(); }

		private:
			rendering(const init_info& info, windowing::windowing& w);
			void init_diligent(GLFWwindow* window, const init_info& info);
			std::unique_ptr<dl::NativeWindow> diligent_window;
			std::unique_ptr<dl::ImGuiImplDiligent> imguiImpl;
		};

	}
}
