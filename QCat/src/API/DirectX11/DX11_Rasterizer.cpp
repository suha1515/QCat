#include "qcpch.h"
#include "DX11_Rasterizer.h"

namespace QCat
{
	namespace Utils
	{
		D3D11_CULL_MODE CullModeToDX(CullMode mode)
		{
			switch (mode)
			{
			case CullMode::None:  return D3D11_CULL_NONE;
			case CullMode::Back:  return D3D11_CULL_BACK;
			case CullMode::Front: return D3D11_CULL_FRONT;
			case CullMode::Front_And_Back: return D3D11_CULL_NONE;
			}
			QCAT_CORE_ASSERT(false, "Wrong Cullmode value.");
		}
		D3D11_FILL_MODE FillModeToDx(FillMode mode)
		{
			switch (mode)
			{
			case FillMode::WireFrame:  return D3D11_FILL_WIREFRAME;
			case FillMode::Solid:	   return D3D11_FILL_SOLID;
			}
			QCAT_CORE_ASSERT(false, "Wrong Fillmode value.");
		}
	}
	DX11RasterizerState::DX11RasterizerState(Rasterizer_Desc desc)
	{
		this->desc = desc;
		Initialize();
	}
	void DX11RasterizerState::SetFillMode(FillMode mode)
	{
		auto exValue = desc.fillmode;
		desc.fillmode = mode;
		if (exValue != mode)
			changed = true;
	}
	void DX11RasterizerState::SetCullMode(CullMode mode)
	{
		auto exValue = desc.cullmode;
		desc.cullmode = mode;
		if (exValue != mode)
			changed = true;
	}
	void DX11RasterizerState::SetFrontClockWise(bool enable)
	{
		auto exValue = desc.frontCounterClockWise;
		desc.frontCounterClockWise = enable;
		if (exValue != enable)
			changed = true;
	}
	void DX11RasterizerState::SetDepthBias(int value)
	{
		auto exValue = desc.DepthBias;
		desc.DepthBias = value;
		if (exValue != value)
			changed = true;
	}
	void DX11RasterizerState::SetDepthBiasClamp(float value)
	{
		auto exValue = desc.DepthBiasClamp;
		desc.DepthBiasClamp = value;
		if (exValue != value)
			changed = true;
	}
	void DX11RasterizerState::SetSlopeScaledDepthBias(float value)
	{
		auto exValue = desc.SlopeScaledDepthBias;
		desc.SlopeScaledDepthBias = value;
		if (exValue != value)
			changed = true;
	}
	void DX11RasterizerState::SetDepthClip(bool enable)
	{
		auto exValue = desc.DepthClipEnable;
		desc.DepthClipEnable = enable;
		if (exValue != enable)
			changed = true;
	}
	void DX11RasterizerState::SetSissor(bool enable)
	{
		auto exValue = desc.ScissorEnable;
		desc.ScissorEnable = enable;
		if (exValue != enable)
			changed = true;
	}
	void DX11RasterizerState::SetMultiSample(bool enable)
	{
		auto exValue = desc.MultisampleEnable;
		desc.MultisampleEnable = enable;
		if (exValue != enable)
			changed = true;
	}
	void DX11RasterizerState::SetAntialiasedLine(bool enable)
	{
		auto exValue = desc.AntialiasedLineEnable;
		desc.AntialiasedLineEnable = enable;
		if (exValue != enable)
			changed = true;
	}
	void DX11RasterizerState::Bind()
	{
		if (changed)
		{
			Initialize();
			changed = false;
		}

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		gfx.GetContext()->RSSetState(pRasterizer.Get());
	}
	void DX11RasterizerState::Initialize()
	{
		if (pRasterizer)
			pRasterizer.Reset();
		//Rasterizer
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.FillMode = Utils::FillModeToDx(desc.fillmode);
		rasterDesc.CullMode = Utils::CullModeToDX(desc.cullmode);
		rasterDesc.FrontCounterClockwise = desc.frontCounterClockWise;
		rasterDesc.DepthBias = desc.DepthBias;
		rasterDesc.DepthBiasClamp = desc.DepthBiasClamp;
		rasterDesc.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
		rasterDesc.DepthClipEnable = desc.DepthClipEnable;
		rasterDesc.ScissorEnable = desc.ScissorEnable;
		rasterDesc.MultisampleEnable = desc.MultisampleEnable;
		rasterDesc.AntialiasedLineEnable = desc.AntialiasedLineEnable;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		gfx.GetDevice()->CreateRasterizerState(&rasterDesc, &pRasterizer);
	}
}