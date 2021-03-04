#pragma once
#include "enums.h"

namespace QCat
{
	enum class STENCIL_OP
	{
		KEEP, ZERO, REPLACE,
		INCREMENT, INCREMENT_WRAP,
		DECREMENT, DECREMENT_WRAP,
		INVERT
	};
	enum class DEPTH_WRITE_MASK
	{
		MASK_ZERO, MASK_ALL
	};
	class DepthStencil
	{
	public:
		struct DepthStencilDesc
		{
			// front face stencil
			COMPARISON_FUNC FrontstencilFunc;
			STENCIL_OP		FrontstencilFail;
			STENCIL_OP		FrontdepthFail;
			STENCIL_OP		FrontbothPass;

			// back face
			COMPARISON_FUNC BackstencilFunc;
			STENCIL_OP		BackstencilFail;
			STENCIL_OP		BackdepthFail;
			STENCIL_OP		BackbothPass;

			unsigned char   stencilReadMask;
			unsigned char   stencilWriteMask;
			bool			stencilEnable = false;

			// depth
			COMPARISON_FUNC  depthFunc;
			DEPTH_WRITE_MASK depthWriteMask;
			bool			 depthEnable = false;

			int referenceValue = 0;
		};
	public:
		DepthStencil() = default;
		virtual ~DepthStencil() = default;
		virtual void EnableDepth(bool enable) = 0;
		virtual void SetDepthFunc(COMPARISON_FUNC func) = 0;
		virtual void SetDepthWriteMask(DEPTH_WRITE_MASK mask) = 0;
		
		virtual void EnableStencil(bool enable) = 0;
		virtual void SetFrontStencilFunc(COMPARISON_FUNC func, int ref, int mask) = 0;
		virtual void SetFrontStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) = 0;
		virtual void SetBackStencilFunc(COMPARISON_FUNC func, int ref, int mask) = 0;
		virtual void SetBackStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) = 0;

		virtual void SetStencilWriteMask(int mask) = 0;
		virtual void SetStencilReadMask(int mask) = 0;
		

		virtual void Bind()=0;

		static Ref<DepthStencil> Create(DepthStencilDesc desc);
	protected:
		DepthStencilDesc desc;
	};
}
