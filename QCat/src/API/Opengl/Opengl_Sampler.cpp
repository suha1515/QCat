#include "qcpch.h"
#include "Opengl_Sampler.h"
#include <glad/glad.h>
namespace QCat
{
	namespace Utils
	{
		GLenum ComparisonToOpengl(COMPARISON_FUNC func)
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
		GLenum GetFilter(Filter filter)
		{
			switch (filter)
			{
			case Filter::MIN: return GL_TEXTURE_MIN_FILTER; break;
			case Filter::MAG: return GL_TEXTURE_MAG_FILTER; break;
				QCAT_CORE_ASSERT(false, "Wrong Filter in Sampler");
			}
		}
		GLenum GetFiltering(Filtering filtering, Filtering mipFiltering = Filtering::NONE)
		{
			if (mipFiltering != Filtering::NONE)
			{
				switch (mipFiltering)
				{
				case Filtering::POINT:
				{
					switch (filtering)
					{
					case Filtering::POINT:  return GL_NEAREST_MIPMAP_NEAREST; break;
					case Filtering::LINEAR: return GL_NEAREST_MIPMAP_LINEAR; break;
						QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
					}
				}
				case Filtering::LINEAR:
				{
					switch (filtering)
					{
					case Filtering::POINT:  return GL_LINEAR_MIPMAP_NEAREST; break;
					case Filtering::LINEAR: return GL_LINEAR_MIPMAP_LINEAR; break;
						QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");

					}
				}
				QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
				}
			}
			else
			{
				switch (filtering)
				{
				case Filtering::POINT:  return GL_NEAREST; break;
				case Filtering::LINEAR: return GL_LINEAR; break;
					QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
				}
			}
		}
		GLenum GetWrapMode(WrapingMode mode)
		{
			switch(mode)
			{
			case WrapingMode::REPEAT: return GL_REPEAT;
			case WrapingMode::MIRROR: return GL_MIRRORED_REPEAT;
			case WrapingMode::MIRROR_ONCE: return GL_MIRROR_CLAMP_TO_EDGE;
			case WrapingMode::BORDER: return GL_CLAMP_TO_BORDER;
			case WrapingMode::CLAMP:  return GL_CLAMP_TO_EDGE;
				QCAT_CORE_ASSERT(false, "Wrong Wrapmode in Sampler");
			}
		}
		void SetSamplerFilter(unsigned int id,GLenum filter, Filtering filtering,Filtering mipFiltering=Filtering::NONE)
		{
			GLenum glfiltering;
			glfiltering = GetFiltering(filtering, mipFiltering);
			
			glSamplerParameteri(id, filter, glfiltering);
		}
		void SetSamplerWrap(unsigned int id,GLenum address, WrapingMode mode)
		{
			GLenum wrapmode = GetWrapMode(mode);

			glSamplerParameteri(id, address, wrapmode);
		}
	}	
	OpenGLSampler::OpenGLSampler(Sampler_Desc desc)
		:SamplerState(desc)
	{
		Invalidate();
	}
	OpenGLSampler::~OpenGLSampler()
	{
		glDeleteSamplers(1, &id);
	}
	void OpenGLSampler::Bind(unsigned int textureId)
	{
		glBindSampler(textureId, id);
	}
	void OpenGLSampler::UnBind()
	{
		glBindSampler(0, 0);
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