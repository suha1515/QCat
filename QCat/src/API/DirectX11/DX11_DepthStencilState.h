#pragma once
#include "QCat/Renderer/DepthStencil.h"
#include "QGfxDeviceDX11.h"
#include <wrl.h>

namespace QCat
{
	class DX11DepthStencilState : public DepthStencil
	{
	public:
		DX11DepthStencilState(DepthStencil::DepthStencilDesc desc);
		virtual ~DX11DepthStencilState() = default;

		virtual void EnableDepth(bool enable) override;
		virtual void SetDepthFunc(COMPARISON_FUNC func) override;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) override;

		virtual void EnableStencil(bool enable) override;
		virtual void SetStencilFunc(COMPARISON_FUNC func, int ref, int mask) override;
		virtual void SetStencilWriteMask(int mask) override;
		virtual void SetStencilReadMask(int mask) override;
		virtual void SetStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) override;


		virtual void Bind() override;
	public:
		void Initialize();
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		bool changed = false;
	};
}
