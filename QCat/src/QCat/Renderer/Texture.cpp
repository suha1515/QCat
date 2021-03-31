#include "qcpch.h"
#include "Texture.h"

#include "Renderer.h"

#include "API/Opengl/OpenGLTexture.h"
#include "API/DirectX11/DX11_Texture.h"

namespace  QCat
{
	Ref<Texture2D> Texture2D::Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples , void* pData)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(format, desc, width, height, mipLevel, samples, pData);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2D>(format, desc, width, height, mipLevel, samples,pData);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples )
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(desc, width, height, mipLevel, samples);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2D>(desc, width, height, mipLevel, samples);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip,bool gamacorrection)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path, desc, mipLevel, samples, flip,gamacorrection);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2D>(path, desc, mipLevel, samples, flip,gamacorrection);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel,bool flip, bool gammacorrection)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLCubeMapTexture>(imagePath, desc, mipLevel, flip, gammacorrection);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11TextureCube>(imagePath, desc, mipLevel, flip, gammacorrection);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, void* pData)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLCubeMapTexture>(format, desc, width,height, mipLevel,pData);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11TextureCube>(format, desc, width, height, mipLevel, pData);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}