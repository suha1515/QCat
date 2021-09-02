#include "qcpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "API/Opengl/OpenGLBuffer.h"
#include "API//Opengl/OpenGL_InputLayout.h"
#include "API/DirectX11/DX11_Buffer.h"
#include "API/DirectX11/DX11_InputLayout.h"

namespace QCat
{
	Ref<VertexBuffer> VertexBuffer::Create(unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
		case RenderAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(size); break;
		case RenderAPI::API::DirectX11: return CreateRef<DX11VertexBuffer>(size); break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
			case RenderAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size); break;
			case RenderAPI::API::DirectX11: return  CreateRef<DX11VertexBuffer>(vertices, size); break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
			case RenderAPI::API::OpenGL:	return  CreateRef<OpenGLIndexBuffer>(indices, size); break;
			case RenderAPI::API::DirectX11: return CreateRef<DX11IndexBuffer>(indices, size); break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<BufferLayout> BufferLayout::Create(const std::initializer_list<BufferElement>& elements, Ref<Shader> vertexShader)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:
		{
			QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
			break;
		}
		case RenderAPI::API::OpenGL:
		{
			return CreateRef<OpenGL_InputLayout>(elements);
			break;
		}
		case RenderAPI::API::DirectX11:
			
			return CreateRef<DX11_InputLayout>(elements, std::dynamic_pointer_cast<DXShader>(vertexShader));
			break;

		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	
	Ref<ShaderBuffer> ShaderBuffer::Create(uint32_t size, uint32_t count, BufferType type, void* pData)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr; break;
		case RenderAPI::API::OpenGL:	return  CreateRef<OpenGLShaderBuffer>(size,count,pData); break;
		case RenderAPI::API::DirectX11: 
			if(type == BufferType::Read)
				return CreateRef<DX11StructureBuffer>(size,count,pData);
			else
				return CreateRef<DX11RWStructureBuffer>(size, count, pData);
			break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}