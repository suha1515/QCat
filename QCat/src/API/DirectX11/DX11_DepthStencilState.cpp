#include "qcpch.h"
#include "DX11_DepthStencilState.h"

namespace QCat
{
	namespace Utils
	{
		D3D11_DEPTH_WRITE_MASK DepthMaskToDX(DEPTH_WRITE_MASK mask)
		{
			switch (mask)
			{
			case DEPTH_WRITE_MASK::MASK_ALL:
				return D3D11_DEPTH_WRITE_MASK_ALL;
				break;
			case DEPTH_WRITE_MASK::MASK_ZERO:
				return D3D11_DEPTH_WRITE_MASK_ZERO;
				break;
			}
			QCAT_CORE_ASSERT(false, "Wrong DepthMask value.");
		}
		D3D11_COMPARISON_FUNC ComparisonFuncToDx(COMPARISON_FUNC func)
		{
			switch (func)
			{
			case COMPARISON_FUNC::ALWAYS:			return D3D11_COMPARISON_ALWAYS;
			case COMPARISON_FUNC::GREATER:		return D3D11_COMPARISON_GREATER;
			case COMPARISON_FUNC::GREATER_EQUAL:  return D3D11_COMPARISON_GREATER_EQUAL;
			case COMPARISON_FUNC::LESS:			return D3D11_COMPARISON_LESS;
			case COMPARISON_FUNC::LESS_EQUAL:		return D3D11_COMPARISON_LESS_EQUAL;
			case COMPARISON_FUNC::NOT_EQUAL:		return D3D11_COMPARISON_NOT_EQUAL;
			case COMPARISON_FUNC::EQUAL:			return D3D11_COMPARISON_EQUAL;
			case COMPARISON_FUNC::NEVER:			return D3D11_COMPARISON_NEVER;
			}
			QCAT_CORE_ASSERT(false, "Wrong ComparisonFunc value.");

		}
		D3D11_STENCIL_OP StencilOpToDx(STENCIL_OP op)
		{
			switch (op)
			{
			case STENCIL_OP::KEEP:				return D3D11_STENCIL_OP_KEEP;
			case STENCIL_OP::ZERO:				return D3D11_STENCIL_OP_ZERO;
			case STENCIL_OP::REPLACE:				return D3D11_STENCIL_OP_REPLACE;
			case STENCIL_OP::INCREMENT:			return D3D11_STENCIL_OP_INCR_SAT;
			case STENCIL_OP::INCREMENT_WRAP:		return D3D11_STENCIL_OP_INCR;
			case STENCIL_OP::DECREMENT:			return D3D11_STENCIL_OP_DECR_SAT;
			case STENCIL_OP::DECREMENT_WRAP:		return D3D11_STENCIL_OP_DECR;
			case STENCIL_OP::INVERT:				return D3D11_STENCIL_OP_INVERT;
			}
			QCAT_CORE_ASSERT(false, "Wrong Stencil Operator value.");

		}
	}
	DX11DepthStencilState::DX11DepthStencilState(DepthStencilDesc desc)
	{
		this->desc = desc;
	}
	void DX11DepthStencilState::EnableDepth(bool enable)
	{
		bool exvalue = desc.depthEnable;
		desc.depthEnable = enable;
		if (exvalue != desc.depthEnable)
			changed = true;
	}
	void DX11DepthStencilState::SetDepthFunc(COMPARISON_FUNC func)
	{
		COMPARISON_FUNC exFunc = desc.depthFunc;
		desc.depthFunc = func;
		if (exFunc != desc.depthFunc)
			changed = true;
	}
	void DX11DepthStencilState::SetDepthWriteMask(DEPTH_WRITE_MASK mask)
	{
		DEPTH_WRITE_MASK exMask = desc.depthWriteMask;
		desc.depthWriteMask = mask;
		if (exMask != desc.depthWriteMask)
			changed = true;
	}
	void DX11DepthStencilState::EnableStencil(bool enable)
	{
		bool exValue = desc.stencilEnable;
		desc.stencilEnable = enable;
		if (exValue != desc.stencilEnable)
			changed = true;
	}
	void DX11DepthStencilState::SetStencilFunc(COMPARISON_FUNC func, int ref, int mask)
	{
		COMPARISON_FUNC exFunc = desc.stencilFunc;
		int exRef = desc.referenceValue;
		desc.stencilFunc = func;
		desc.referenceValue = ref;
		if (exFunc != desc.stencilFunc && exRef != desc.referenceValue)
			changed = true;

	}
	void DX11DepthStencilState::SetStencilWriteMask(int mask)
	{
		int exMask = desc.stencilWriteMask;
		desc.stencilWriteMask = mask;
		if (exMask != desc.stencilWriteMask)
			changed = true;
	}
	void DX11DepthStencilState::SetStencilReadMask(int mask)
	{
		int exMask = desc.stencilReadMask;
		desc.stencilReadMask = mask;
		if (exMask != desc.stencilReadMask)
			changed = true;
	}
	void DX11DepthStencilState::SetStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		STENCIL_OP exOp1 = desc.stencilFail;
		STENCIL_OP exOp2 = desc.depthFail;
		STENCIL_OP exOp3 = desc.bothPass;

		desc.stencilFail = stencilFail;
		desc.depthFail = depthFail;
		desc.bothPass = bothPass;

		if (exOp1 != desc.stencilFail && exOp2 != desc.depthFail && exOp3 != desc.bothPass)
			changed = true;

	}
	void DX11DepthStencilState::Bind()
	{
		if (changed)
			Initialize();

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		gfx.GetContext()->OMSetDepthStencilState(pDSState.Get(), desc.referenceValue);
	}
	void DX11DepthStencilState::Initialize()
	{
		if (pDSState)
			pDSState.Reset();
		//Depth-StencilState
		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

		// Depth test parameters
		dsDesc.DepthEnable = desc.depthEnable;
		dsDesc.DepthWriteMask = Utils::DepthMaskToDX(desc.depthWriteMask);
		dsDesc.DepthFunc = Utils::ComparisonFuncToDx(desc.depthFunc);

		// Stencil test parameters
		dsDesc.StencilEnable = desc.stencilEnable;
		dsDesc.StencilReadMask = desc.stencilReadMask;
		dsDesc.StencilWriteMask = desc.stencilWriteMask;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = Utils::StencilOpToDx(desc.stencilFail);
		dsDesc.FrontFace.StencilDepthFailOp = Utils::StencilOpToDx(desc.depthFail);
		dsDesc.FrontFace.StencilPassOp = Utils::StencilOpToDx(desc.bothPass);
		dsDesc.FrontFace.StencilFunc = Utils::ComparisonFuncToDx(desc.stencilFunc);

		// Stencil operations if pixel is back-facing
		// later
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		gfx.GetDevice()->CreateDepthStencilState(&dsDesc, &pDSState);
	}
}