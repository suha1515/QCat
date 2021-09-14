#include "qcpch.h"
#include "Opengl_Sampler.h"
#include <glad/glad.h>
#include "API/Opengl/OpenglUtils.h"

namespace QCat
{
	OpenGLSampler::OpenGLSampler(Sampler_Desc desc)
		:SamplerState(desc)
	{
		Invalidate();
	}
	OpenGLSampler::~OpenGLSampler()
	{
		glDeleteSamplers(1, &id);
	}
	void OpenGLSampler::Bind(unsigned int textureId, ShaderType type )
	{
		if (IsChanged)
		{
			Invalidate();
			IsChanged = false;
		}
		glBindSampler(textureId, id);
	}
	void OpenGLSampler::UnBind(unsigned int slot, ShaderType type )
	{
		glBindSampler(slot, 0);
	}
	void OpenGLSampler::Invalidate()
	{
		if (id > 0)
		{
			glDeleteSamplers(1, &id);
		}
		GLenum error;
		glGenSamplers(1, &id);
		Utils::SetSamplerFilter(id, GL_TEXTURE_MIN_FILTER, desc.MIN, desc.MIP);
		Utils::SetSamplerFilter(id, GL_TEXTURE_MAG_FILTER, desc.MAG);
		Utils::SetSamplerWrap(id, GL_TEXTURE_WRAP_S, desc.addressU);
		Utils::SetSamplerWrap(id, GL_TEXTURE_WRAP_T, desc.addressV);
		Utils::SetSamplerWrap(id, GL_TEXTURE_WRAP_R, desc.addressW);

		if (desc.mode == FilterMode::COMPARISON)
		{
			glSamplerParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glSamplerParameteri(id, GL_TEXTURE_COMPARE_FUNC, Utils::ComparisonToOpengl(desc.comparisonFunc));
		}
		else
			glSamplerParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		// min level of detail
		glSamplerParameterf(id, GL_TEXTURE_MIN_LOD, desc.minLod);
		// max level of detail
		glSamplerParameterf(id, GL_TEXTURE_MAX_LOD, desc.maxLod);
		// mip level of detail bias
		glSamplerParameterf(id, GL_TEXTURE_LOD_BIAS, desc.MipLodBias);
		// border color
		glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, desc.borderColor);
	}
}