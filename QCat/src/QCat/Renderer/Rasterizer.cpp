#include "qcpch.h"
#include "Rasterizer.h"
#include "RenderAPI.h"

#include "API/DirectX11/DX11_Rasterizer.h"
#include "API/Opengl/OpenGL_Rasterizer.h"

namespace QCat
{
	Ref<Rasterizer> Rasterizer::Create(Rasterizer_Desc desc)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: QCAT_CORE_ASSERT(false, "Rasterizer need RenderAPI information"); return nullptr;
		case RenderAPI::API::OpenGL:	return CreateRef<OpenGLRasterizer>(desc);
		case RenderAPI::API::DirectX11: return CreateRef<DX11RasterizerState>(desc);
		}
		QCAT_CORE_ASSERT(false, "Error! Creating Rasterizer!");
		return nullptr;
	}
}