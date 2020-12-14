#include "qcpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "API/Opengl/OpenGL_VertexArray.h"

namespace QCat
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:			return new OpenGLVertexArray();
		case RenderAPI::API::DirectX11:		return nullptr;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}