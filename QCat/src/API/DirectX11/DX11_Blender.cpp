#include "qcpch.h"
#include "DX11_Blender.h"

namespace QCat
{
	namespace Utils
	{
		D3D11_BLEND_OP BlendOpToDx(BlendOp op)
		{
			switch (op)
			{
			case BlendOp::BLEND_ADD:				return D3D11_BLEND_OP_ADD;
			case BlendOp::BLEND_SUBTRACT:			return D3D11_BLEND_OP_SUBTRACT;
			case BlendOp::BLEND_MIN:				return D3D11_BLEND_OP_MIN;
			case BlendOp::BLEND_MAX:				return D3D11_BLEND_OP_MAX;
			case BlendOp::BLEND_REVERSE_SUBTRACT:	return D3D11_BLEND_OP_REV_SUBTRACT;
			}
			QCAT_CORE_ASSERT(false, "BlendOp Type Error!");
		}
		D3D11_BLEND BlendFuncToDx(BlendFunc func)
		{
			switch (func)
			{
			case BlendFunc::BLEND_ZERO:				return D3D11_BLEND_ZERO;
			case BlendFunc::BLEND_ONE:				return D3D11_BLEND_ONE;
			case BlendFunc::BLEND_SRC_COLOR:		return D3D11_BLEND_SRC_COLOR;
			case BlendFunc::BLEND_INV_SRC_COLOR:	return D3D11_BLEND_INV_SRC_COLOR;
			case BlendFunc::BLEND_DST_COLOR:		return D3D11_BLEND_DEST_COLOR;
			case BlendFunc::BLEND_INV_DST_COLOR:	return D3D11_BLEND_INV_DEST_COLOR;
			case BlendFunc::BLEND_SRC_ALPHA:		return D3D11_BLEND_SRC_ALPHA;
			case BlendFunc::BLEND_INV_SRC_ALPHA:	return D3D11_BLEND_INV_SRC_ALPHA;
			case BlendFunc::BLEND_DST_ALPHA:		return D3D11_BLEND_DEST_ALPHA;
			case BlendFunc::BLEND_INV_DST_ALPHA:	return D3D11_BLEND_INV_DEST_ALPHA;
			case BlendFunc::BLEND_SRC1_COLOR:		return D3D11_BLEND_SRC1_COLOR;
			case BlendFunc::BLEND_SRC1_ALPHA:		return D3D11_BLEND_SRC1_ALPHA;
			case BlendFunc::BLEND_INV_SRC1_COLOR:	return D3D11_BLEND_INV_SRC1_COLOR;
			case BlendFunc::BLEND_INV_SRC1_ALPHA:	return D3D11_BLEND_INV_SRC1_ALPHA;

			}
			QCAT_CORE_ASSERT(false, "BlendFunc Type Error!");
		}
	}
	DX11Blender::DX11Blender()
	{
		Blend_Target_Desc temp;
		temp.blendEnable = true;
		temp.srcBlend = BlendFunc::BLEND_ONE;
		temp.dstBlend = BlendFunc::BLEND_ZERO;
		temp.blendOp = BlendOp::BLEND_ADD;
		temp.srcBlendAlpha = BlendFunc::BLEND_ONE;
		temp.dstBlendAlpha = BlendFunc::BLEND_ZERO;
		temp.blendOpAlpha = BlendOp::BLEND_ADD;
		temp.writeMask = ColorWriteMask::COLOR_WRITE_ALL;
		Blender::desc.BlendDesc[0] = temp;
		Blender::desc.AlphaToCoverage = false;
		Blender::desc.IndependentBlendEnable = false;
		BlendDesc desc = Blender::desc;

		blendDesc.AlphaToCoverageEnable = desc.AlphaToCoverage;
		blendDesc.IndependentBlendEnable = desc.IndependentBlendEnable;

		for (int i = 0; i < 8; ++i)
		{
			blendDesc.RenderTarget[i].BlendEnable = desc.BlendDesc[i].blendEnable;
			blendDesc.RenderTarget[i].SrcBlend = Utils::BlendFuncToDx(desc.BlendDesc[i].srcBlend);
			blendDesc.RenderTarget[i].DestBlend = Utils::BlendFuncToDx(desc.BlendDesc[i].dstBlend);
			blendDesc.RenderTarget[i].BlendOp = Utils::BlendOpToDx(desc.BlendDesc[i].blendOp);

			blendDesc.RenderTarget[i].SrcBlendAlpha = Utils::BlendFuncToDx(desc.BlendDesc[i].srcBlendAlpha);
			blendDesc.RenderTarget[i].DestBlendAlpha = Utils::BlendFuncToDx(desc.BlendDesc[i].dstBlendAlpha);
			blendDesc.RenderTarget[i].BlendOpAlpha = Utils::BlendOpToDx(desc.BlendDesc[i].blendOpAlpha);
		}

		Invalidate();
	}
	DX11Blender::~DX11Blender()
	{
	}
	void DX11Blender::Invalidate()
	{
		if (pBlender)
			pBlender.Reset();	

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &pBlender);
	}
	void DX11Blender::Bind() const
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
	}
	void DX11Blender::SetEnableBlend(uint32_t index, bool enable)
	{
		QCAT_CORE_ASSERT(index < 8, "Index is over : Blend Desc");
		if (desc.BlendDesc[index].blendEnable != enable)
		{
			desc.BlendDesc[index].blendEnable = enable;

			blendDesc.RenderTarget[index].BlendEnable = desc.BlendDesc[index].blendEnable;
			Invalidate();
		}
	}
	void DX11Blender::SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp)
	{
		QCAT_CORE_ASSERT(index < 8, "Index is over : Blend Desc");
		if (desc.BlendDesc[index].blendEnable)
		{
			if (desc.BlendDesc[index].blendOp != colorOp || desc.BlendDesc[index].blendOpAlpha != alphaOp)
			{
				desc.BlendDesc[index].blendOp = colorOp;
				desc.BlendDesc[index].blendOpAlpha = alphaOp;
				
				blendDesc.RenderTarget[index].BlendOp = Utils::BlendOpToDx(desc.BlendDesc[index].blendOp);
				blendDesc.RenderTarget[index].BlendOpAlpha = Utils::BlendOpToDx(desc.BlendDesc[index].blendOpAlpha);

				Invalidate();
			}
		}
	}
	void DX11Blender::SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		QCAT_CORE_ASSERT(index < 8, "Index is over : Blend Desc");
		if (desc.BlendDesc[index].blendEnable)
		{
			if (desc.BlendDesc[index].srcBlend != srcColor || desc.BlendDesc[index].dstBlend != dstColor
				|| desc.BlendDesc[index].srcBlendAlpha != srcAlpha || desc.BlendDesc[index].dstBlendAlpha != dstAlpha)
			{
				desc.BlendDesc[index].srcBlend = srcColor;
				desc.BlendDesc[index].dstBlend = dstColor;
				desc.BlendDesc[index].srcBlendAlpha = srcAlpha;
				desc.BlendDesc[index].dstBlendAlpha = dstAlpha;

				blendDesc.RenderTarget[index].SrcBlend			= Utils::BlendFuncToDx(desc.BlendDesc[index].srcBlend);
				blendDesc.RenderTarget[index].DestBlend			= Utils::BlendFuncToDx(desc.BlendDesc[index].dstBlend);
				blendDesc.RenderTarget[index].SrcBlendAlpha		= Utils::BlendFuncToDx(desc.BlendDesc[index].srcBlendAlpha);
				blendDesc.RenderTarget[index].DestBlendAlpha	= Utils::BlendFuncToDx(desc.BlendDesc[index].dstBlendAlpha);

				Invalidate();
			}
		}
	}
	void DX11Blender::SetIndependentBlend(bool enable)
	{
		if (desc.IndependentBlendEnable != enable)
		{
			desc.IndependentBlendEnable = enable;
			blendDesc.IndependentBlendEnable = desc.IndependentBlendEnable;

			Invalidate();
		}
	}
	void DX11Blender::SetAlphaToCoverage(bool enable)
	{
		if (desc.AlphaToCoverage != enable)
		{
			desc.AlphaToCoverage = enable;
			blendDesc.AlphaToCoverageEnable = desc.AlphaToCoverage;
			Invalidate();
		}
	}
}