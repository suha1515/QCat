#include "qcpch.h"
#include "Blend.h"
#include "QCat/Renderer/RenderAPI.h"

#include "API/DirectX11/DX11_Blender.h"
#include "API/Opengl/OpenGL_Blender.h"

namespace QCat
{
	 Ref<Blender> Blender::Create()
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: QCAT_CORE_ASSERT(false, "BlenderState need RenderAPI information"); return nullptr;
		case RenderAPI::API::OpenGL: return CreateRef<OpenGLBlender>();
		case RenderAPI::API::DirectX11: return CreateRef<DX11Blender>();
		}
		QCAT_CORE_ASSERT(false, "Error! Creating BlenderState!");
		return nullptr;
	}
}
