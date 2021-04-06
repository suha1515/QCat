#include "qcpch.h"
#include "RenderAPI.h"

namespace QCat
{
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::None;
#if defined(QCAT_OPENGL)
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::OpenGL;
#elif defined(QCAT_DX11)
	RenderAPI::API RenderAPI::s_api = RenderAPI::API::DirectX11;
#endif
}