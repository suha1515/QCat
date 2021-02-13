#include "qcpch.h"
#include "OpenGL_Blender.h"
#include <glad/glad.h>
namespace QCat
{
	namespace Utils
	{
		GLenum BlendOpToGl(BlendOp op)
		{
			switch (op)
			{
			case BlendOp::BLEND_ADD:				return GL_FUNC_ADD;
			case BlendOp::BLEND_SUBTRACT:			return GL_FUNC_SUBTRACT;
			case BlendOp::BLEND_MIN:				return GL_MIN;
			case BlendOp::BLEND_MAX:				return GL_MAX;
			case BlendOp::BLEND_REVERSE_SUBTRACT:	return GL_FUNC_REVERSE_SUBTRACT;
			}
			QCAT_CORE_ASSERT(false, "BlendOp Type Error!");
		}
		GLenum BlendFuncToGl(BlendFunc func)
		{
			switch (func)
			{
			case BlendFunc::BLEND_ZERO:				return GL_ZERO;
			case BlendFunc::BLEND_ONE:				return GL_ONE;
			case BlendFunc::BLEND_SRC_COLOR:		return GL_SRC_COLOR;
			case BlendFunc::BLEND_INV_SRC_COLOR:	return GL_ONE_MINUS_SRC_COLOR;
			case BlendFunc::BLEND_DST_COLOR:		return GL_DST_COLOR;
			case BlendFunc::BLEND_INV_DST_COLOR:	return GL_ONE_MINUS_DST_COLOR;
			case BlendFunc::BLEND_SRC_ALPHA:		return GL_SRC_ALPHA;
			case BlendFunc::BLEND_INV_SRC_ALPHA:	return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFunc::BLEND_DST_ALPHA:		return GL_DST_ALPHA;
			case BlendFunc::BLEND_INV_DST_ALPHA:	return GL_ONE_MINUS_DST_ALPHA;
			case BlendFunc::BLEND_SRC1_COLOR:		return GL_SRC1_COLOR;
			case BlendFunc::BLEND_SRC1_ALPHA:		return GL_SRC1_ALPHA;
			case BlendFunc::BLEND_INV_SRC1_COLOR:	return GL_ONE_MINUS_SRC1_COLOR;
			case BlendFunc::BLEND_INV_SRC1_ALPHA:	return GL_ONE_MINUS_SRC1_ALPHA;

			}
			QCAT_CORE_ASSERT(false, "BlendFunc Type Error!");
		}
	}
	OpenGLBlender::OpenGLBlender()
	{
		Blend_Target_Desc desc;
		desc.blendEnable = false;
		desc.srcBlend = BlendFunc::BLEND_ONE;
		desc.dstBlend = BlendFunc::BLEND_ZERO;
		desc.blendOp = BlendOp::BLEND_ADD;
		desc.srcBlendAlpha = BlendFunc::BLEND_ONE;
		desc.dstBlendAlpha = BlendFunc::BLEND_ZERO;
		desc.blendOpAlpha = BlendOp::BLEND_ADD;
		desc.writeMask = ColorWriteMask::COLOR_WRITE_ALL;
		Blender::desc.BlendDesc[0] = desc;

	}
	void OpenGLBlender::SetEnableBlend(uint32_t index, bool enable)
	{
		QCAT_CORE_ASSERT(index < 8, "Index is over : Blend Desc");
		if (enable)
		{
			glEnablei(GL_BLEND,index);
		}
		else
			glDisablei(GL_BLEND,index);
	}
	void OpenGLBlender::SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp)
	{
		QCAT_CORE_ASSERT(index < 8, "Index is over : Blend Desc");
		glBlendEquationSeparatei(index, Utils::BlendOpToGl(colorOp), Utils::BlendOpToGl(alphaOp));
	}
	void OpenGLBlender::SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		GLenum srcRgb = Utils::BlendFuncToGl(srcColor);
		GLenum dstRgb = Utils::BlendFuncToGl(dstColor);
		GLenum sourceAlpha = Utils::BlendFuncToGl(srcAlpha);
		GLenum destAlpha = Utils::BlendFuncToGl(dstAlpha);

		glBlendFuncSeparatei(index, srcRgb, dstRgb, sourceAlpha, destAlpha);
	}
	void OpenGLBlender::SetIndependentBlend(bool enable)
	{
		if (desc.IndependentBlendEnable != enable)
		{
			desc.IndependentBlendEnable = enable;
		}
	}
	void OpenGLBlender::SetAlphaToCoverage(bool enable)
	{
		if (desc.AlphaToCoverage != enable)
		{
			desc.AlphaToCoverage = enable;
		}
	}
}

