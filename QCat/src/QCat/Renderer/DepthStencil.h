#pragma once

namespace QCat
{
	enum class STENCIL_OP
	{
		KEEP, ZERO, REPLACE,
		INCREMENT, INCREMENT_WRAP,
		DECREMENT, DECREMENT_WRAP,
		INVERT
	};
	enum class COMPARISON_FUNC
	{
		NEVER, EQUAL,
		LESS, LESS_EQUAL,
		GREATER, GREATER_EQUAL,
		NOT_EQUAL, ALWAYS
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
			// stencil
			COMPARISON_FUNC stencilFunc;
			STENCIL_OP		stencilFail;
			STENCIL_OP		depthFail;
			STENCIL_OP		bothPass;
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
		virtual void SetStencilFunc(COMPARISON_FUNC func, int ref, int mask) = 0;
		virtual void SetStencilWriteMask(int mask) = 0;
		virtual void SetStencilReadMask(int mask) = 0;
		virtual void SetStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass) = 0;

		virtual void Bind()=0;

		static Ref<DepthStencil> Create(DepthStencilDesc desc);
	protected:
		DepthStencilDesc desc;
	};
}
