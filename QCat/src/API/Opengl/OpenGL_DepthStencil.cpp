#include "qcpch.h"
#include "OpenGL_DepthStencil.h"
#include <glad/glad.h>

namespace QCat
{
	namespace Utils
	{
		GLenum ComparisonFuncToOpengl(COMPARISON_FUNC func)
		{
			switch (func)
			{
			case COMPARISON_FUNC::ALWAYS:			return GL_ALWAYS;
			case COMPARISON_FUNC::GREATER:			return GL_GREATER;
			case COMPARISON_FUNC::GREATER_EQUAL:	return GL_GEQUAL;
			case COMPARISON_FUNC::LESS:				return GL_LESS;
			case COMPARISON_FUNC::LESS_EQUAL:		return GL_LEQUAL;
			case COMPARISON_FUNC::NOT_EQUAL:		return GL_NOTEQUAL;
			case COMPARISON_FUNC::EQUAL:			return GL_EQUAL;
			case COMPARISON_FUNC::NEVER:			return GL_NEVER;
			}
			QCAT_CORE_ASSERT(false, "Wrong ComparisonFunc value.");

		}
		GLenum StencilOpToOpengl(STENCIL_OP op)
		{
			switch (op)
			{
			case STENCIL_OP::KEEP:				return GL_KEEP;
			case STENCIL_OP::ZERO:				return GL_ZERO;
			case STENCIL_OP::REPLACE:			return GL_REPLACE;
			case STENCIL_OP::INCREMENT:			return GL_INCR;
			case STENCIL_OP::INCREMENT_WRAP:	return GL_INCR_WRAP;
			case STENCIL_OP::DECREMENT:			return GL_DECR;
			case STENCIL_OP::DECREMENT_WRAP:	return GL_DECR_WRAP;
			case STENCIL_OP::INVERT:			return GL_INVERT;
			}
			QCAT_CORE_ASSERT(false, "Wrong Stencil Operator value.");
		}
	}
	void OpenGLDepthStencil::EnableDepth(bool enable)
	{
		desc.depthEnable = enable;
		if (desc.depthEnable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	void OpenGLDepthStencil::SetDepthFunc(COMPARISON_FUNC func)
	{
		desc.depthFunc = func;
		glDepthFunc(Utils::ComparisonFuncToOpengl(func));
	}
	void OpenGLDepthStencil::SetDepthWriteMask(DEPTH_WRITE_MASK mask)
	{
		desc.depthWriteMask = mask;
		if (desc.depthWriteMask == DEPTH_WRITE_MASK::MASK_ALL)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}
	void OpenGLDepthStencil::EnableStencil(bool enable)
	{
		desc.stencilEnable = enable;
		if (desc.stencilEnable)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}

	void OpenGLDepthStencil::SetStencilWriteMask(int mask)
	{
		desc.stencilWriteMask = mask;
		glStencilMask(desc.stencilWriteMask);
	}
	void OpenGLDepthStencil::SetStencilReadMask(int mask)
	{
		desc.stencilReadMask = mask;
	}
	void OpenGLDepthStencil::SetFrontStencilFunc(COMPARISON_FUNC func, int ref, int mask)
	{
		desc.FrontstencilFunc = func;
		desc.referenceValue = ref;
		glStencilFuncSeparate(GL_FRONT,Utils::ComparisonFuncToOpengl(desc.FrontstencilFunc), desc.referenceValue, 0xFF);
	}
	void OpenGLDepthStencil::SetFrontStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		desc.FrontstencilFail = stencilFail;
		desc.FrontdepthFail	 = depthFail;
		desc.FrontbothPass    = bothPass;

		glStencilOpSeparate(GL_FRONT,Utils::StencilOpToOpengl(desc.FrontstencilFail), Utils::StencilOpToOpengl(desc.FrontdepthFail), Utils::StencilOpToOpengl(desc.FrontbothPass));

	}
	void OpenGLDepthStencil::SetBackStencilFunc(COMPARISON_FUNC func, int ref, int mask)
	{
		desc.BackstencilFunc = func;
		desc.referenceValue = ref;
		glStencilFuncSeparate(GL_BACK, Utils::ComparisonFuncToOpengl(desc.BackstencilFunc), desc.referenceValue, 0xFF);
	}
	void OpenGLDepthStencil::SetBackStencilOperator(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		desc.BackstencilFail = stencilFail;
		desc.BackdepthFail = depthFail;
		desc.BackbothPass = bothPass;

		glStencilOpSeparate(GL_BACK, Utils::StencilOpToOpengl(desc.BackstencilFail), Utils::StencilOpToOpengl(desc.BackdepthFail), Utils::StencilOpToOpengl(desc.BackbothPass));
	}
	void OpenGLDepthStencil::Bind()
	{
	}
}