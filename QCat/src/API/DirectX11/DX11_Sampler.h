#pragma once
#include "QGfxDeviceDX11.h"
#include "QCat/Renderer/SamplerState.h"

namespace QCat
{
	class DX11Sampler : public SamplerState
	{
	public:
		DX11Sampler(Sampler_Desc desc);
		virtual ~DX11Sampler();

		virtual void Bind(unsigned int slot) override;
		virtual void UnBind(unsigned int slot) override;

		void Invalidate();
	private:
		Microsoft::WRL::ComPtr< ID3D11SamplerState> pSamplerState;
	};
}