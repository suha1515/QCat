#include "qcpch.h"
#include "DX11_Blender.h"

namespace QCat
{
	DX11Blender::DX11Blender()
	{
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		// for blendcolor not based on alpha,based on specified user value (float4 color)
		//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
		//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &pBlender);
	}
	DX11Blender::~DX11Blender()
	{
	}
	void DX11Blender::Bind() const
	{
		const float* data = nullptr;
		std::array<float, 4> colorArray = {0.5f,0.5f,0.5f,0.5f};
		QGfxDeviceDX11::GetInstance()->GetContext()->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
	}
}