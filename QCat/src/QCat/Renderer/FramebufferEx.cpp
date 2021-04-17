#include "qcpch.h"
#include "FramebufferEx.h"
#include "Renderer.h"

#include "API/Opengl/OpenGL_FrameBufferEx.h"
#include "API/DirectX11/DX11_FrameBufferEx.h"

namespace QCat
{
	Ref<FrameBufferEx> FrameBufferEx::Create(AttachmentSpecification attachments)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
		case RenderAPI::API::OpenGL:	return CreateRef<OpenGLFrameBufferEx>(attachments); break;
		case RenderAPI::API::DirectX11: return CreateRef<DirectXFrameBufferEx>(attachments); break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}