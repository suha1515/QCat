#include "qcpch.h"
#include "OpenGL_RenderAPI.h"

#include <glad/glad.h>
namespace QCat
{
	namespace Utils
	{
		GLenum DrawModetoGL(RenderAPI::DrawMode mode)
		{
			switch (mode)
			{
			case RenderAPI::DrawMode::POINT:			return GL_POINTS;
			case RenderAPI::DrawMode::LINES:			return GL_LINES;
			case RenderAPI::DrawMode::LINE_LOOP:		return GL_LINE_LOOP;
			case RenderAPI::DrawMode::LINE_STRIP:		return GL_LINE_STRIP;
			case RenderAPI::DrawMode::TRIANGLES:		return GL_TRIANGLES;
			case RenderAPI::DrawMode::TRIANGLES_FAN:	return GL_TRIANGLE_FAN;
			case RenderAPI::DrawMode::TRIANGLE_STRIP:	return GL_TRIANGLE_STRIP;
			}
		}
	}
	void GLAPIENTRY
		MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
		{
			fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
				"** GL ERROR **",type, severity, message);
		}
		
	}
	void OpenGLRenderAPI::Init(uint32_t width, uint32_t height)
	{
		QCAT_PROFILE_FUNCTION();

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		DepthStencil::DepthStencilDesc desc;
		{
			desc.depthEnable = true;
			desc.depthFunc = COMPARISON_FUNC::LESS;
			desc.depthWriteMask = DEPTH_WRITE_MASK::MASK_ALL;

			desc.stencilEnable = true;
			desc.FrontstencilFunc = COMPARISON_FUNC::ALWAYS;
			desc.stencilReadMask = 0xFF;
			desc.stencilWriteMask = 0xFF;
			desc.FrontstencilFail = STENCIL_OP::KEEP;
			desc.FrontdepthFail = STENCIL_OP::KEEP;
			desc.FrontbothPass = STENCIL_OP::REPLACE;

			desc.BackstencilFunc = COMPARISON_FUNC::ALWAYS;
			desc.BackstencilFail = STENCIL_OP::KEEP;
			desc.BackdepthFail = STENCIL_OP::KEEP;
			desc.BackbothPass = STENCIL_OP::REPLACE;

			desc.referenceValue = 1;
		}
		m_DepthStencilState = DepthStencil::Create(desc);
		m_DepthStencilState->EnableDepth(true);
		m_DepthStencilState->EnableStencil(false);
		m_DepthStencilState->SetFrontStencilOperator(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::REPLACE);
		m_DepthStencilState->SetBackStencilOperator(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::REPLACE);
		SetDepthFunc(COMPARISON_FUNC::LESS);

		m_BlenderState = Blender::Create();
		m_BlenderState->SetIndependentBlend(false);

		Rasterizer::Rasterizer_Desc Rastdesc;
		m_RasterizeState = Rasterizer::Create(Rastdesc);

		SetCullMode(CullMode::Back);
		SetFrontClockWise(false);


#ifdef QCAT_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#else

#endif

	}
	void OpenGLRenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRenderAPI::SetDefaultFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLRenderAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
	}
	void OpenGLRenderAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, unsigned int indexCount, DrawMode mode)
	{
		unsigned int count = indexCount ?  indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(Utils::DrawModetoGL(mode), count,GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindTextureUnit(0, 0);
	}
	
	void OpenGLRenderAPI::SetDepthTest(bool enable)
	{
		m_DepthStencilState->EnableDepth(enable);
	}
	void OpenGLRenderAPI::SetDepthFunc(COMPARISON_FUNC func)
	{
		m_DepthStencilState->SetDepthFunc(func);
	}
	void OpenGLRenderAPI::SetStencilTest(bool enable)
	{
		m_DepthStencilState->EnableStencil(enable);
	}
	void OpenGLRenderAPI::SetFrontStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		m_DepthStencilState->SetFrontStencilOperator(stencilFail, depthFail, bothPass);
	}
	void OpenGLRenderAPI::SetFrontStencilFunc(COMPARISON_FUNC func, int value)
	{
		m_DepthStencilState->SetFrontStencilFunc(func,value,0xFF);
	}
	void OpenGLRenderAPI::SetBackStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		m_DepthStencilState->SetBackStencilOperator(stencilFail, depthFail, bothPass);
	}
	void OpenGLRenderAPI::SetBackStencilFunc(COMPARISON_FUNC func, int value)
	{
		m_DepthStencilState->SetBackStencilFunc(func, value, 0xFF);
	}
	void OpenGLRenderAPI::SetDepthWriteMask(DEPTH_WRITE_MASK mask)
	{
		m_DepthStencilState->SetDepthWriteMask(mask);
	}
	void OpenGLRenderAPI::SetStencilWriteMask(int value)
	{
		m_DepthStencilState->SetStencilWriteMask(value);
	}
	void OpenGLRenderAPI::SetFillMode(FillMode mode)
	{
		m_RasterizeState->SetFillMode(mode);
	}
	void OpenGLRenderAPI::SetCullMode(CullMode mode)
	{
		m_RasterizeState->SetCullMode(mode);
	}
	void OpenGLRenderAPI::SetFrontClockWise(bool enable)
	{
		m_RasterizeState->SetFrontClockWise(enable);
	}
	void OpenGLRenderAPI::SetDepthBias(int value)
	{
		m_RasterizeState->SetDepthBias(value);
	}
	void OpenGLRenderAPI::SetDepthBiasClamp(float value)
	{
		m_RasterizeState->SetDepthBiasClamp(value);
	}
	void OpenGLRenderAPI::SetSlopeScaledDepthBias(float value)
	{
		m_RasterizeState->SetSlopeScaledDepthBias(value);
	}
	void OpenGLRenderAPI::SetDepthClip(bool enable)
	{
		m_RasterizeState->SetDepthClip(enable);
	}
	void OpenGLRenderAPI::SetSissor(bool enable)
	{
		m_RasterizeState->SetSissor(enable);
	}
	void OpenGLRenderAPI::SetMultiSample(bool enable)
	{
		m_RasterizeState->SetMultiSample(enable);
	}
	void OpenGLRenderAPI::SetAntialiasedLine(bool enable)
	{
		m_RasterizeState->SetAntialiasedLine(enable);
	}
	void OpenGLRenderAPI::SetBlend(bool enable)
	{
		m_BlenderState->SetEnableBlend(0,enable);
	}
	void OpenGLRenderAPI::SetBlend(uint32_t index, bool enable)
	{
		m_BlenderState->SetEnableBlend(index, enable);
	}
	void OpenGLRenderAPI::SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		m_BlenderState->SetBlendFunc(0,srcColor,dstColor,srcAlpha,dstAlpha);
	}
	void OpenGLRenderAPI::SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		m_BlenderState->SetBlendFunc(index, srcColor, dstColor, srcAlpha, dstAlpha);
	}
	void OpenGLRenderAPI::SetBlendOp(BlendOp colorOp, BlendOp alphaOp)
	{
		m_BlenderState->SetBlendOp(0,colorOp, alphaOp);
	}
	void OpenGLRenderAPI::SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp)
	{
		m_BlenderState->SetBlendOp(index, colorOp, alphaOp);
	}

}