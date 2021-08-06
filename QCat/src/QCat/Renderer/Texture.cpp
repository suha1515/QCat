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
	Ref<Texture2D> Texture2D::Create(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip,bool gamacorrection)
	{

		auto begin = path.find_last_of('.');
		std::string extension = path.substr(begin + 1, path.length());
		if (extension == "jpg" || extension == "png" || extension == "tga" || extension == "jpeg"
			|| extension == "hdr" || extension == "ibl")
		{
			switch (Renderer::GetAPI())
			{
			case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
			case RenderAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path, desc, mipLevel, samples, flip, gamacorrection);
			case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2D>(path, desc, mipLevel, samples, flip, gamacorrection);
			}
			QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
		else
		{
			QCAT_CORE_ERROR("Texture2D Create Error : Wrong Texture Extension {0} ", extension);
			return nullptr;
		}
		

	}
	Ref<Texture2DArray> Texture2DArray::Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel, unsigned int samples )
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGlTexture2DArray>(format, desc, width,height,depth, mipLevel, samples);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2DArray>(format, desc, width, height, depth, mipLevel, samples);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<Texture2DArray> Texture2DArray::Create(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples , bool flip, bool gamacorrection )
	{
		bool allcheck = true;
		for (auto path : imagePath)
		{
			auto begin = path.find_last_of('.');
			std::string extension = path.substr(begin + 1, path.length());
			if (extension == "jpg" || extension == "png" || extension == "tga" || extension == "jpeg"
				|| extension == "hdr" || extension == "ibl")
			{			
			}
			else
				allcheck = false;
		}
		if (allcheck)
		{
			switch (Renderer::GetAPI())
			{
			case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
			case RenderAPI::API::OpenGL:		return CreateRef<OpenGlTexture2DArray>(imagePath, desc, mipLevel, samples, flip, gamacorrection);
			case RenderAPI::API::DirectX11:		return CreateRef<DX11Texture2DArray>(imagePath, desc, mipLevel, samples, flip, gamacorrection);
			}
			QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
		else
		{
			QCAT_CORE_ERROR("Texture2DArray Create Error : Wrong Texture Extension");
			return nullptr;
		}		
	}

	Ref<TextureCube> TextureCube::Create(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel,bool flip, bool gammacorrection)
	{
		bool allcheck = true;
		for (auto path : imagePath)
		{
			auto begin = path.find_last_of('.');
			std::string extension = path.substr(begin + 1, path.length());
			if (extension == "jpg" || extension == ".png" || extension == ".tga" || extension == "jpeg"
				|| extension == "hdr" || extension == "ibl")
			{
			}
			else
				allcheck = false;
		}
		if (allcheck)
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
		else
		{
			QCAT_CORE_ERROR("Texture2DArray Create Error : Wrong Texture Extension");
			return nullptr;
		}
	}

	Ref<TextureCube> TextureCube::Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, void* pData)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLCubeMapTexture>(format, desc, width,height, mipLevel);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11TextureCube>(format, desc, width, height, mipLevel, pData);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<TextureCubeArray> TextureCubeArray::Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel , unsigned int samples )
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLCubeMapArray>(format, desc, width, height,depth, mipLevel,samples);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11TextureCubeArray>(format, desc, width, height,depth, mipLevel,samples);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<TextureUtility> TextureUtility::Get_()
	{
		static Ref<TextureUtility> util;
		if (util == nullptr)
		{
			switch (Renderer::GetAPI())
			{
			case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); break;
			case RenderAPI::API::OpenGL:		util = CreateRef<OpenGLTextureUtility>(); break;
			case RenderAPI::API::DirectX11:		util = CreateRef<DX11TextureUtility>(); break;
			default:
				QCAT_CORE_ASSERT(false, "Unknown RenderAPI!"); break;
			}		
		}
		return util;
	}

	Ref<TextureShaderView> TextureShaderView::Create(TextureType type, const Ref<Texture>& originTexture, TextureFormat foramt, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLTextureShaderView>(type, originTexture, foramt, startMip, numMip, startLayer,numlayer);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11ShaderView>(type, originTexture, foramt, startMip, numMip, startLayer, numlayer);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<RenderTargetView> RenderTargetView::Create(TextureType type, const Ref<Texture>& originTexture, TextureFormat foramt, uint32_t startMip, uint32_t startLayer, uint32_t numlayer)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLRenderTargetView>(type, originTexture, foramt, startMip, startLayer, numlayer);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11RenderTargetView>(type, originTexture, foramt, startMip, startLayer, numlayer);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<DepthStencilView> DepthStencilView::Create(TextureType type, const Ref<Texture>& originTexture, TextureFormat foramt, uint32_t startMip,uint32_t startLayer, uint32_t numlayer)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return CreateRef<OpenGLDepthStencilView>(type, originTexture, foramt, startMip, startLayer, numlayer);
		case RenderAPI::API::DirectX11:		return CreateRef<DX11DepthStencilView>(type, originTexture, foramt, startMip, startLayer, numlayer);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}



	

}