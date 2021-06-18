#include "qcpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "API/Opengl/OpenGL_VertexArray.h"
#include "API/DirectX11/DX11_VertexArray.h"

namespace QCat
{
	Ref<VertexArray> VertexArray::Create(const std::string& meshname)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return std::make_shared<OpenGLVertexArray>(meshname);
		case RenderAPI::API::DirectX11:		return std::make_shared<DX11VertexArray>(meshname);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}