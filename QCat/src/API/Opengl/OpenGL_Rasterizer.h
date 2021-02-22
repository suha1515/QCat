#pragma once
#include "QCat/Renderer/Rasterizer.h"

namespace QCat
{
	class OpenGLRasterizer : public Rasterizer
	{
	public:
		OpenGLRasterizer(Rasterizer_Desc desc);
		virtual ~OpenGLRasterizer() = default;

		virtual void SetFillMode(FillMode mode) override;
		virtual void SetCullMode(CullMode mode) override;
		virtual void SetClockWise(bool enable) override;
		virtual void SetDepthBias(int value) override;
		virtual void SetDepthBiasClamp(float value) override;
		virtual void SetSlopeScaledDepthBias(float value) override;
		virtual void SetDepthClip(bool enable) override;
		virtual void SetSissor(bool enable) override;
		virtual void SetMultiSample(bool enable) override;
		virtual void SetAntialiasedLine(bool enable) override;

		virtual void Bind() override;
	private:
	
	};

}