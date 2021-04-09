#pragma once
#include "QCat/Renderer/RenderAPI.h"
#include "QCat/Renderer/FrameBuffer.h"

namespace QCat
{
	class QGfxDeviceDX11;
	class DX11RenderAPI : public RenderAPI
	{
	public:
		DX11RenderAPI();
		~DX11RenderAPI();
	public:
		virtual void Init() override;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
		virtual void ResizeFrameBuffer(unsigned int width, unsigned int height) override;
		virtual void SetDefaultFrameBuffer() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const unsigned int indexCount);
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0, DrawMode mode = DrawMode::TRIANGLES) override;
	
		// depth-stencil option
		virtual void SetDepthTest(bool enable)override;
		virtual void SetDepthFunc(COMPARISON_FUNC func)override;
		virtual void SetStencilTest(bool enable) override;
		virtual void SetFrontStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)override;
		virtual void SetFrontStencilFunc(COMPARISON_FUNC func, int value) override;
		virtual void SetBackStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) override;
		virtual void SetBackStencilFunc(COMPARISON_FUNC func, int value) override;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) override;
		virtual void SetStencilWriteMask(int value) override;

		// Rasterize State Function
		virtual void SetFillMode(FillMode mode)override;
		virtual void SetCullMode(CullMode mode)override;
		virtual void SetFrontClockWise(bool enable) override;
		virtual void SetDepthBias(int value) override;
		virtual void SetDepthBiasClamp(float value) override;
		virtual void SetSlopeScaledDepthBias(float value) override;
		virtual void SetDepthClip(bool enable) override;
		virtual void SetSissor(bool enable) override;
		virtual void SetMultiSample(bool enable) override;
		virtual void SetAntialiasedLine(bool enable) override;

		//blend option
		virtual void SetBlend(bool enable) override;
		virtual void SetBlend(uint32_t index, bool enable) override;
		virtual void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) override;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) override;
		virtual void SetBlendOp(BlendOp colorOp, BlendOp alphaOp) override;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) override;

		//Defualt FrameBuffer
		virtual void* GetDefaultFrameBuffer() override ;
	private:
		QGfxDeviceDX11* pgfx;
		glm::vec4 color;
		Ref<DepthStencil> m_DepthStencilState;
		Ref<Blender> m_BlenderState;
		Ref<Rasterizer> m_RasterizState;

		Ref<FrameBuffer> m_FrameBuffer;
	};
}