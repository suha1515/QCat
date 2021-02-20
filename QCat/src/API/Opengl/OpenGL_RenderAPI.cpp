#include "qcpch.h"
#include "OpenGL_RenderAPI.h"

#include <glad/glad.h>
namespace QCat
{
	void OpenGLRenderAPI::Init()
	{
		QCAT_PROFILE_FUNCTION();

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glFrontFace(GL_CW);
		glDepthFunc(GL_LEQUAL);
		
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
		m_BlenderState = Blender::Create();
		m_BlenderState->SetIndependentBlend(false);
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
	void OpenGLRenderAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, unsigned int indexCount)
	{
		unsigned int count = indexCount ?  indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count,GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void OpenGLRenderAPI::SetWireFrameMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	void OpenGLRenderAPI::SetFillMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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