#include "qcpch.h"
#include "SamplerState.h"
#include "RenderAPI.h"

#include "API/Opengl/Opengl_Sampler.h"
#include "API/DirectX11/DX11_Sampler.h"

namespace QCat
{
    Ref<SamplerState> QCat::SamplerState::Create(Sampler_Desc desc)
    {
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: QCAT_CORE_ASSERT(false, "DepthStencilState need RenderAPI information"); return nullptr;
		case RenderAPI::API::OpenGL: return CreateRef<OpenGLSampler>(desc);
		case RenderAPI::API::DirectX11: return CreateRef<DX11Sampler>(desc);
		}
		QCAT_CORE_ASSERT(false, "Error! Creating SamplerState");
		return nullptr;
    }
}

