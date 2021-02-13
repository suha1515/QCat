#pragma once
#include "VertexArray.h"
#include "DepthStencil.h"
#include "Blend.h"

namespace QCat
{
	class RenderAPI
	{
	public:
		enum class API
		{
			None =0,OpenGL = 1,DirectX11 = 2
		};
	public:
		virtual ~RenderAPI() = default;

		virtual void Init()=0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
		virtual void SetDefaultFrameBuffer()=0;
		virtual void SetClearColor(const glm::vec4& color)=0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0)=0;
		virtual void SetWireFrameMode() = 0;
		virtual void SetFillMode() = 0;

		virtual void SetDepthTest(bool enable)=0;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) = 0;
		virtual void SetStencilTest(bool enable)=0;
		virtual void SetStencilOp(STENCIL_OP stencilFail,STENCIL_OP depthFail,STENCIL_OP bothPass) = 0;
		virtual void SetStencilFunc(COMPARISON_FUNC func,int value)=0;
		virtual void SetStencilWriteMask(int value) = 0;

		virtual void SetBlend( bool enable) = 0;
		virtual void SetBlend(uint32_t index, bool enable) = 0;
		virtual void SetBlendFunc( BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) = 0;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) = 0;
		virtual void SetBlendOp( BlendOp colorOp, BlendOp alphaOp) = 0;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) = 0;

		inline static API GetAPI() { return s_api; }
	private:
		static API s_api;
		
	};
}