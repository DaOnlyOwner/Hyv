#include "rendering/rendering.h"
#include "rendering/render_systems.h"
#include <utility>

#if PLATFORM_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

#ifdef D3D12_SUPPORTED
#include "EngineFactoryD3D12.h"
#endif

#if VULKAN_SUPPORTED
#include <EngineFactoryVk.h>
#endif
#include <algorithm>
#include <thread>
#include "logging.h"

#include "imgui_impl_glfw.h"

//#include "imgui.h"
//#include "DiligentTools/Imgui/interface/ImGuiImplDiligent.hpp"
//#include "DiligentTools/Imgui/interface/ImGuiUtils.hpp"

std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> hyv::DeferredCtxts;
Diligent::RefCntAutoPtr<Diligent::IDeviceContext> hyv::Imm;
Diligent::RefCntAutoPtr<Diligent::ISwapChain> hyv::SwapChain;
Diligent::RefCntAutoPtr<Diligent::IRenderDevice> hyv::Dev;
Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> hyv::ShaderStream;
std::vector<Diligent::RefCntAutoPtr<Diligent::ICommandList>> hyv::CmdLists;

void callback(Diligent::DEBUG_MESSAGE_SEVERITY Severity,
	const char* Message,
	const char* Function,
	const char* File,
	int                         Line)
{
	if (Severity == Diligent::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR)
	{
		HYV_ERROR("{}", Message);
	}

	else if (Severity == Diligent::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_ERROR)
	{
		HYV_NON_FATAL_ERROR("{}", Message);
	}

	else if (Severity == Diligent::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_INFO)
	{
		HYV_INFO("{}", Message);
	}

	else if (Severity == Diligent::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_WARNING)
	{
		HYV_WARN("{}", Message);
	}
}



hyv::rendering::rendering::rendering(const init_info& info, windowing::windowing& w)
{
	init_diligent(w.get_window(),info);
}

void hyv::rendering::rendering::new_frame()
{
	Imm->ClearRenderTarget(SwapChain->GetCurrentBackBufferRTV(), clear_color, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	Imm->ClearDepthStencil(SwapChain->GetDepthBufferDSV(), dl::CLEAR_DEPTH_FLAG, 1.f, 0, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	ImGui_ImplGlfw_NewFrame();
	imguiImpl->NewFrame(SwapChain->GetDesc().Width, SwapChain->GetDesc().Height, SwapChain->GetDesc().PreTransform);
}

void hyv::rendering::rendering::end_frame()
{
	auto* rtv = SwapChain->GetCurrentBackBufferRTV();
	auto* dsv = SwapChain->GetDepthBufferDSV();
	Imm->SetRenderTargets(1, &rtv, dsv, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	imguiImpl->Render(Imm);
	Imm->Flush();
	Imm->FinishFrame();
	SwapChain->Present();
}

hyv::rendering::rendering::~rendering()
{
	ImGui_ImplGlfw_Shutdown();
}

hyv::rendering::rendering& hyv::rendering::rendering::inst(const init_info& info, windowing::windowing& w)
{
	static rendering instance(info, w);
	return instance;
}

hyv::rendering::rendering& hyv::rendering::rendering::inst()
{
	return inst({}, windowing::windowing::inst());
}

void hyv::rendering::rendering::init_diligent(GLFWwindow* window, const init_info& info)
{
	// Allocate at least two deferred contexts
	//dl::SetDebugMessageCallback(callback);
	hyv::DeferredCtxts.resize((std::max)(std::thread::hardware_concurrency() - 1, 1u));
	std::vector<dl::IDeviceContext*> ctxts;
	ctxts.reserve(hyv::DeferredCtxts.size() + 1);
	ctxts.push_back(hyv::Imm);
	for (int i = 0; i < hyv::DeferredCtxts.size(); i++)
	{
		ctxts.push_back(hyv::DeferredCtxts[i]);
	}

	dl::SwapChainDesc sdesc;
	void* window_handle;
#if PLATFORM_WIN32
	window_handle = glfwGetWin32Window(window);
#elif PLATFORM_LINUX
	window_handle = glfwGetX11Window(window);
#else 
	static_assert(false, "OSX is not supported");
#endif
	diligent_window = std::make_unique<dl::NativeWindow>(window_handle);
	switch (info.backend)
	{
	case hyv::init_info::RenderBackend::Vulkan:
	{
		auto getVk = dl::LoadGraphicsEngineVk();
		dl::EngineVkCreateInfo vkCi;
		if (info.enableDebugLayers)
		{
			vkCi.EnableValidation = info.enableDebugLayers;
			vkCi.SetValidationLevel(dl::VALIDATION_LEVEL_2);
		}
		vkCi.NumDeferredContexts = DeferredCtxts.size();

		//vkCi.NumImmediateContexts = 1;
		auto* vkFactory = getVk();
		vkFactory->SetMessageCallback(callback);
		vkFactory->CreateDeviceAndContextsVk(vkCi, &Dev, ctxts.data());
		vkFactory->CreateSwapChainVk(Dev.RawPtr(), ctxts[0], sdesc, *diligent_window, &SwapChain);
		vkFactory->CreateDefaultShaderSourceStreamFactory(SHADER_RES, &ShaderStream);
	}
	break;

	case hyv::init_info::RenderBackend::D3D12:
	{
		auto getD3 = dl::LoadGraphicsEngineD3D12();
		dl::EngineD3D12CreateInfo D3Ci;
		if (info.enableDebugLayers)
		{
			D3Ci.EnableValidation = info.enableDebugLayers;
			D3Ci.SetValidationLevel(dl::VALIDATION_LEVEL_2);
		}
		D3Ci.NumDeferredContexts = DeferredCtxts.size();
		//D3Ci.NumImmediateContexts = 1;	
		auto* d3Factory = getD3();
		d3Factory->SetMessageCallback(callback);
		d3Factory->CreateDeviceAndContextsD3D12(D3Ci, &Dev, ctxts.data());
		d3Factory->CreateSwapChainD3D12(Dev.RawPtr(), ctxts[0], sdesc, dl::FullScreenModeDesc{}, *diligent_window, &SwapChain);
		d3Factory->CreateDefaultShaderSourceStreamFactory(SHADER_RES, &ShaderStream);
	}
	break;
	default:
		break;
	}

	Imm = ctxts[0];
	for (int i = 1; i < ctxts.size(); i++)
	{
		DeferredCtxts[i - 1] = ctxts[i];
	}

	

	//Renderer::Inst().Init(device, imm, swapChain,streamFactory);
	imguiImpl = std::make_unique<dl::ImGuiImplDiligent>(Dev, SwapChain->GetDesc().ColorBufferFormat, SwapChain->GetDesc().DepthBufferFormat);
}
