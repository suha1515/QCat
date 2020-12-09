#include "qcpch.h"
#include "Renderer.h"

namespace QCat
{
#if defined(QCAT_OPENGL)
	RenderAPI Renderer::s_renderAPI = RenderAPI::OpenGL;
#elif defined(QCAT_DX11)
	RenderAPI Renderer::s_renderAPI = RenderAPI::DirectX11;
#else
	RenderAPI Renderer::s_renderAPI = RenderAPI::None;
#endif
}