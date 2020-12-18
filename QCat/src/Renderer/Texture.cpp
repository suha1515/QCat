#include "qcpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "API/Opengl/OpenGLTexture.h"
#include "API/DirectX11/DX11_Texture.h"
namespace  QCat
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:			QCAT_CORE_ASSERT(false, "RenderAPI isnt selected! : VertexArray Error"); return nullptr;
		case RenderAPI::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(path);
		case RenderAPI::API::DirectX11:		return std::make_shared<DX11Texture2D>(path);
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}