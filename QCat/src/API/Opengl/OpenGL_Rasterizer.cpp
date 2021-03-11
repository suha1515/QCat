#include "qcpch.h"
#include "OpenGL_Rasterizer.h"
#include <glad/glad.h>

namespace QCat
{
	namespace Utils
	{
		GLenum CullModeToGL(CullMode mode)
		{
			switch (mode)
			{
			case CullMode::Back:		   return GL_BACK;
			case CullMode::Front:		   return GL_FRONT;
			case CullMode::Front_And_Back: return GL_FRONT_AND_BACK;
				QCAT_CORE_ASSERT(false, "Wrong Cullmode value.");

			}
		}
		GLenum FillModeToGL(FillMode mode)
		{
			switch (mode)
			{
			case FillMode::Solid:	  return GL_FILL;
			case FillMode::WireFrame: return GL_LINE;
				QCAT_CORE_ASSERT(false, "Wrong Fillmode value.");

			}
		}
	}
	OpenGLRasterizer::OpenGLRasterizer(Rasterizer_Desc desc)
	{
		this->desc = desc;
	}
	void OpenGLRasterizer::SetFillMode(FillMode mode)
	{
		GLenum fillmode = Utils::FillModeToGL(mode);
		GLenum face = Utils::CullModeToGL(desc.cullmode);
		desc.fillmode = mode;
		glPolygonMode(face, fillmode);
	}
	void OpenGLRasterizer::SetCullMode(CullMode mode)
	{
		if (mode == CullMode::None)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			GLenum cullMode = Utils::CullModeToGL(mode);
			desc.cullmode = mode;
			glCullFace(cullMode);
		}
	}
	void OpenGLRasterizer::SetFrontClockWise(bool enable)
	{
		desc.frontCounterClockWise = enable;
		if (desc.frontCounterClockWise)
			glFrontFace(GL_CCW);
		else
			glFrontFace(GL_CW);
	}
	void OpenGLRasterizer::SetDepthBias(int value)
	{
		int exvalue = value;
		if (desc.DepthBias != value)
		{
			desc.DepthBias = value;
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffsetClamp(desc.SlopeScaledDepthBias, desc.DepthBias,desc.DepthBiasClamp);
		}
	}
	void OpenGLRasterizer::SetDepthBiasClamp(float value)
	{
		int exvalue = value;
		if (desc.DepthBiasClamp != value)
		{
			desc.DepthBiasClamp = value;
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffsetClamp(desc.SlopeScaledDepthBias, desc.DepthBias, desc.DepthBiasClamp);
		}
	}
	void OpenGLRasterizer::SetSlopeScaledDepthBias(float value)
	{
		int exvalue = value;
		if (desc.SlopeScaledDepthBias != value)
		{
			desc.SlopeScaledDepthBias = value;
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffsetClamp(desc.SlopeScaledDepthBias, desc.DepthBias, desc.DepthBiasClamp);
		}
	}
	void OpenGLRasterizer::SetDepthClip(bool enable)
	{
		desc.DepthClipEnable = enable;
	}
	void OpenGLRasterizer::SetSissor(bool enable)
	{
		desc.ScissorEnable = enable;
	}
	void OpenGLRasterizer::SetMultiSample(bool enable)
	{
		desc.MultisampleEnable = enable;
	}
	void OpenGLRasterizer::SetAntialiasedLine(bool enable)
	{
		desc.AntialiasedLineEnable = enable;
	}
	void OpenGLRasterizer::Bind()
	{
	}
}