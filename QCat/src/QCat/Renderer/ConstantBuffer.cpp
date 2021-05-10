#include "qcpch.h"
#include "ConstantBuffer.h"

#include "QCat/Renderer/Renderer.h"
#include "API/Opengl/OpenGL_UniformBuffer.h"
#include "API/DirectX11/DX11_ConstantBuffer.h"

namespace QCat
{
	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    QCAT_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		case RenderAPI::API::DirectX11: return CreateRef<DXConstantBuffer>(size, binding);
		}

		QCAT_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}