#pragma once
#include "QCat/Renderer/Rasterizer.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11RasterizerState : public Rasterizer
	{
	public:
		DX11RasterizerState(Rasterizer_Desc desc);
		virtual ~DX11RasterizerState() {};

		virtual void SetFillMode(FillMode mode)override;
		virtual void SetCullMode(CullMode mode)override;
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
		void Initialize();
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool changed = false;
	};

}