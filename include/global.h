#pragma once
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "Definitions.h"
#include "SwapChain.h"
#include "RefCntAutoPtr.hpp"
#include <vector>

namespace hyv
{
	namespace dl = Diligent;
	extern dl::RefCntAutoPtr<dl::IRenderDevice> Dev;
	extern std::vector<dl::RefCntAutoPtr<dl::IDeviceContext>> DeferredCtxts;
	extern dl::RefCntAutoPtr<dl::IDeviceContext> Imm;
	extern dl::RefCntAutoPtr<dl::ISwapChain> SwapChain;
	extern dl::RefCntAutoPtr<dl::IShaderSourceInputStreamFactory> ShaderStream;
}