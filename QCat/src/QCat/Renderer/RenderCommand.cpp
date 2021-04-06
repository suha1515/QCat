#include "qcpch.h"
#include "RenderCommand.h"



namespace QCat
{
#if defined(QCAT_OPENGL)
	RenderAPI* RenderCommand::s_renderAPI = new OpenGLRenderAPI;
#elif defined(QCAT_DX11)
	RenderAPI* RenderCommand::s_renderAPI = new DX11RenderAPI;
#endif
	RenderAPI* RenderCommand::s_renderAPI = nullptr;
}