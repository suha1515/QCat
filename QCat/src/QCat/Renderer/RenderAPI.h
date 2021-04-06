#pragma once
#include "VertexArray.h"
#include "DepthStencil.h"
#include "Rasterizer.h"
#include "Blend.h"
#include "enums.h"

namespace QCat
{
	class RenderAPI
	{
	public:
		enum class API
		{
			None =0,OpenGL = 1,DirectX11 = 2
		};
		enum class DrawMode
		{
			POINT=0,		// independent point
			LINE_STRIP,		// line is connected
			LINE_LOOP,		// after line_strip start,end point will be connected with line
			LINES,
			TRIANGLES,		// independent triangles
			TRIANGLE_STRIP, // connected triangles
			TRIANGLES_FAN,  // triangles share center
		};
	public:
		virtual ~RenderAPI() = default;

		virtual void Init()=0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
		virtual void SetDefaultFrameBuffer()=0;
		virtual void SetClearColor(const glm::vec4& color)=0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0, DrawMode mode = DrawMode::TRIANGLES)=0;

		// Depth Stencil Function
		virtual void SetDepthTest(bool enable)=0;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) = 0;
		virtual void SetDepthFunc(COMPARISON_FUNC func)=0;
		virtual void SetStencilTest(bool enable)=0;
		virtual void SetFrontStencilOp(STENCIL_OP stencilFail,STENCIL_OP depthFail,STENCIL_OP bothPass) = 0;
		virtual void SetFrontStencilFunc(COMPARISON_FUNC func,int value)=0;
		virtual void SetBackStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) = 0;
		virtual void SetBackStencilFunc(COMPARISON_FUNC func, int value) = 0;
		virtual void SetStencilWriteMask(int value) = 0;

		// Rasterize State Function
		virtual void SetFillMode(FillMode mode) = 0;
		virtual void SetCullMode(CullMode mode) = 0;
		virtual void SetFrontClockWise(bool enable) = 0;
		virtual void SetDepthBias(int value) = 0;
		virtual void SetDepthBiasClamp(float value) = 0;
		virtual void SetSlopeScaledDepthBias(float value) = 0;
		virtual void SetDepthClip(bool enable) = 0;
		virtual void SetSissor(bool enable) = 0;
		virtual void SetMultiSample(bool enable) = 0;
		virtual void SetAntialiasedLine(bool enable) = 0;

		// Blend State Function
		virtual void SetBlend( bool enable) = 0;
		virtual void SetBlend(uint32_t index, bool enable) = 0;
		virtual void SetBlendFunc( BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) = 0;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) = 0;
		virtual void SetBlendOp( BlendOp colorOp, BlendOp alphaOp) = 0;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) = 0;

		// Default FrameBuffer
		virtual void* GetDefaultFrameBuffer() = 0;

		inline static API GetAPI() { return s_api; }
		inline static void SetRenderAPI(API api) { s_api = api; }
	private:
		static API s_api;
		
	};
}