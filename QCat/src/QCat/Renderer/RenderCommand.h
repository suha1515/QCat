#pragma once
#include "RenderAPI.h"

namespace QCat
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_renderAPI->Init();
		}
		inline static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			s_renderAPI->SetViewport(x, y, width, height);
		}
		inline static void SetDefaultFrameBuffer()
		{
			s_renderAPI->SetDefaultFrameBuffer();
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_renderAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_renderAPI->Clear();
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray,unsigned int count =0)
		{
			s_renderAPI->DrawIndexed(vertexArray, count);
		}
		inline static void SetWireFrameMode()
		{
			s_renderAPI->SetWireFrameMode();
		}
		inline static void SetFillMode()
		{
			s_renderAPI->SetFillMode();
		}
		inline static void SetDepthTest(bool enable)
		{
			s_renderAPI->SetDepthTest(enable);
		}
		inline static void SetStencilTest(bool enable)
		{
			s_renderAPI->SetStencilTest(enable);
		}
		inline static void SetStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
		{
			s_renderAPI->SetStencilOp(stencilFail, depthFail, bothPass);
		}
		inline static void SetStencilFunc(COMPARISON_FUNC func, int value)
		{
			s_renderAPI->SetStencilFunc(func, value);
		}

		inline static void SetDepthWriteMask(DEPTH_WRITE_MASK mask)
		{
			s_renderAPI->SetDepthWriteMask(mask);
		}
		inline static void SetStencilWriteMask(int value)
		{
			s_renderAPI->SetStencilWriteMask(value);
		
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