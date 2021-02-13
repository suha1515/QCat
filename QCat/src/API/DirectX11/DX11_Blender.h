#pragma once
#include "QCat/Renderer/Blend.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11Blender : public Blender
	{
	public:
		DX11Blender();
		~DX11Blender();

		void Invalidate();

		virtual void Bind()const override ;
		virtual void UnBind()const override {};

		virtual void SetEnableBlend(uint32_t index, bool enable) override;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) override;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) override;
		virtual void SetIndependentBlend(bool enable) override;
		virtual void SetAlphaToCoverage(bool enable) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
	};
}
