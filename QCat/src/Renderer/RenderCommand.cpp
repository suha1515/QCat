#include "qcpch.h"
#include "RenderCommand.h"

#include "API/Opengl/OpenGL_RenderAPI.h"
#include "API/DirectX11/DX11_RenderAPI.h"
namespace QCat
{
#if defined(QCAT_OPENGL)
	RenderAPI* RenderCommand::s_renderAPI = new OpenGLRenderAPI;
#elif defined(QCAT_DX11)
	RenderAPI* RenderCommand::s_renderAPI = new DX11RenderAPI;
#endif
}