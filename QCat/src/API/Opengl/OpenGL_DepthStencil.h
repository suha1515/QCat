#pragma once
#include "QCat/Renderer/DepthStencil.h"

namespace QCat
{
	class OpenGLDepthStencil : public DepthStencil
	{
	public:
		OpenGLDepthStencil() = default;
		virtual ~OpenGLDepthStencil() = default;

		virtual void EnableDepth(bool enable) override;
		virtual void SetDepthFunc(COMPARISON_FUNC func) override;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) override;

		virtual void EnableStencil(bool enable) override;
		virtual void SetStencilFunc(COMPARISON_FUNC func, int ref, int mask) override;
		virtual void SetStencilWriteMask(int mask) override;
		virtual void SetStencilReadMask(int mask) override;
		virtual void SetStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) override;
		virtual void Bind() override;
	private:
	};
}