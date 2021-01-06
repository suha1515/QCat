#include "qcpch.h"
#include "FrameBuffer.h"
#include "Renderer.h"

#include "API/Opengl/OpenGL_FrameBuffer.h"
#include "API/DirectX11/DX11_FrameBuffer.h"
namespace QCat
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
		case RenderAPI::API::OpenGL:	return CreateRef<OpenGLFrameBuffer>(spec); break;
		case RenderAPI::API::DirectX11: return CreateRef<DX11FrameBuffer>(spec); break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}