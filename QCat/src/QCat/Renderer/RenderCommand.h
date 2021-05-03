#pragma once
#include "RenderAPI.h"
#include "enums.h"

#include "API/Opengl/OpenGL_RenderAPI.h"
#include "API/DirectX11/DX11_RenderAPI.h"
namespace QCat
{
	class RenderCommand
	{
	public:
		inline static void Init(uint32_t width, uint32_t height)
		{
			if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
				s_renderAPI = new DX11RenderAPI;
			else if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
				s_renderAPI = new OpenGLRenderAPI;
			s_renderAPI->Init(width, height);
		}
		inline static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			s_renderAPI->SetViewport(x, y, width, height);
		}
		inline static void ResizeDefaultFramebuffer(unsigned int width, unsigned int height)
		{
			s_renderAPI->ResizeFrameBuffer(width, height);
		}
		inline static void SetDefaultFrameBuffer()
		{
			s_renderAPI->SetDefaultFrameBuffer();
			s_renderAPI->Clear();
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_renderAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_renderAPI->Clear();
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray,unsigned int count =0,RenderAPI::DrawMode mode = RenderAPI::DrawMode::TRIANGLES)
		{
			vertexArray->Bind();
			s_renderAPI->DrawIndexed(vertexArray, count,mode);
		}
		// Depth Stencil State Function
		inline static void SetDepthTest(bool enable)
		{
			s_renderAPI->SetDepthTest(enable);
		}
		inline static void SetDepthFunc(COMPARISON_FUNC func)
		{
			s_renderAPI->SetDepthFunc(func);
		}
		inline static void SetStencilTest(bool enable)
		{
			s_renderAPI->SetStencilTest(enable);
		}
		inline static void SetFrontStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
		{
			s_renderAPI->SetFrontStencilOp(stencilFail, depthFail, bothPass);
		}
		inline static void SetFrontStencilFunc(COMPARISON_FUNC func, int value)
		{
			s_renderAPI->SetFrontStencilFunc(func, value);
		}
		inline static void SetBackStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
		{
			s_renderAPI->SetBackStencilOp(stencilFail, depthFail, bothPass);
		}
		inline static void SetBackStencilFunc(COMPARISON_FUNC func, int value)
		{
			s_renderAPI->SetBackStencilFunc(func, value);
		}
		inline static void SetDepthWriteMask(DEPTH_WRITE_MASK mask)
		{
			s_renderAPI->SetDepthWriteMask(mask);
		}
		inline static void SetStencilWriteMask(int value)
		{
			s_renderAPI->SetStencilWriteMask(value);
		
		}
		// Rasterize State Function
		inline static void SetFillMode(FillMode mode)
		{
			s_renderAPI->SetFillMode(mode);
		}
		inline static void SetCullMode(CullMode mode)
		{
			s_renderAPI->SetCullMode(mode);
		}
		inline static void SetFrontClockWise(bool enable)
		{
			s_renderAPI->SetFrontClockWise(enable);
		}
		inline static void SetDepthBias(int value)
		{
			s_renderAPI->SetDepthBias(value);
		}
		inline static void SetDepthBiasClamp(float value)
		{
			s_renderAPI->SetDepthBiasClamp(value);
		}
		inline static void SetSlopeScaledDepthBias(float value)
		{
			s_renderAPI->SetSlopeScaledDepthBias(value);
		}
		inline static void SetDepthClip(bool enable)
		{
			s_renderAPI->SetDepthClip(enable);
		}
		inline static void SetSissor(bool enable)
		{
			s_renderAPI->SetSissor(enable);
		}
		inline static void SetMultiSample(bool enable)
		{
			s_renderAPI->SetMultiSample(enable);
		}
		inline static void SetAntialiasedLine(bool enable)
		{
			s_renderAPI->SetAntialiasedLine(enable);
		}
		//Blend
		inline static void SetBlend(bool enable)
		{
			s_renderAPI->SetBlend(enable);
		}
		inline static void SetBlend(uint32_t index, bool enable)
		{
			s_renderAPI->SetBlend(index, enable);
		}
		inline static void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
		{
			s_renderAPI->SetBlendFunc(srcColor, dstColor, srcAlpha, dstAlpha);
		}
		inline static void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
		{
			s_renderAPI->SetBlendFunc(index, srcColor, dstColor, srcAlpha, dstAlpha);
		}
		inline static void SetBlendOp(BlendOp colorOp, BlendOp alphaOp)
		{
			s_renderAPI->SetBlendOp(colorOp, alphaOp);
		}
		inline static void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp)
		{
			s_renderAPI->SetBlendOp(index, colorOp, alphaOp);
		}

	private:
		static RenderAPI* s_renderAPI;
	};
}