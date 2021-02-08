#include "qcpch.h"
#include "DepthStencil.h"
#include "RenderAPI.h"

#include "API/Opengl/OpenGL_DepthStencil.h"
#include "API/DirectX11/DX11_DepthStencilState.h"
namespace QCat
{
	Ref<DepthStencil> DepthStencil::Create(DepthStencilDesc desc)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: QCAT_CORE_ASSERT(false, "DepthStencilState need RenderAPI information"); return nullptr;
		case RenderAPI::API::OpenGL: return CreateRef<OpenGLDepthStencil>(); 
		case RenderAPI::API::DirectX11: return CreateRef<DX11DepthStencilState>(desc);
		}
		QCAT_CORE_ASSERT(false, "Error! Creating DepthStencilState!");
		return nullptr;
	}
}

