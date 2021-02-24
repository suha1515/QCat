#include "qcpch.h"
#include "OpenGL_FrameBuffer.h"

#include <glad/glad.h>

namespace QCat
{
	// Temporary code
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils{

		static GLenum GetDataType(FramebufferTextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case FramebufferTextureDataFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case FramebufferTextureDataFormat::RED32_INTEGER:   return GL_INT;
				//24 bit
			case FramebufferTextureDataFormat::RGB8:			return GL_UNSIGNED_BYTE;

			}
		}
		static GLenum GetTextureTarget(FramebufferTextureFormat format,bool multisampled)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Texture2D: return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			case FramebufferTextureFormat::Depth:	  return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			case FramebufferTextureFormat::CubeMap:   return GL_TEXTURE_CUBE_MAP;
			}
		}
		static GLenum GetTextureFormat(FramebufferTextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case FramebufferTextureDataFormat::RGBA8:			return GL_RGBA;
			case FramebufferTextureDataFormat::RED32_INTEGER:   return GL_RED_INTEGER;
				//24 bit
			case FramebufferTextureDataFormat::RGB8:			return GL_RGB;

			}

		}
		static GLenum GetTextureInternalFormat(FramebufferTextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case FramebufferTextureDataFormat::RGBA8:			return GL_RGBA8;
			case FramebufferTextureDataFormat::RED32_INTEGER:   return GL_R32I;
				//24 bit
			case FramebufferTextureDataFormat::RGB8:			return GL_RGB8;
			}
		}

	/*	static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}*/
		static void CreateTextures(FramebufferTextureFormat format,bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(GetTextureTarget(format,multisampled), count, outID);
		}
		static void BindTexture(FramebufferTextureFormat format,bool multisampled,uint32_t id)
		{
			glBindTexture(GetTextureTarget(format,multisampled), id);
		}
		static void AttachColorTexture(FramebufferTextureFormat textureformat, FramebufferTextureDataFormat dataformat,
			uint32_t id, int samples, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GetTextureInternalFormat(dataformat), width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(dataformat), width, height, 0, GetTextureFormat(dataformat), GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GetTextureTarget(textureformat,multisampled), id, 0);

		}
		static void AttachColorTextureCube(FramebufferTextureDataFormat dataformat,uint32_t id, uint32_t width, uint32_t height, int index)
		{
			for (int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetTextureInternalFormat(dataformat), width, height, 0, GetTextureFormat(dataformat), GL_UNSIGNED_BYTE, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X, id, 0);

		}
		static void AttachDepthTexture(FramebufferTextureFormat textureformat,uint32_t id, int samples, GLenum format,GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GetTextureTarget(textureformat, multisampled), id, 0);

		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Depth: return true;
			}
			return false;
		}
		static GLenum QCatTextureFormatToGL(FramebufferTextureDataFormat data)
		{
			switch (data)
			{
			case FramebufferTextureDataFormat::RGBA8:			    return GL_RGBA;
			case FramebufferTextureDataFormat::RED32_INTEGER:		return GL_RED_INTEGER;
			}
			QCAT_CORE_ASSERT(false);
			return 0;
		}
	
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttacmentSpecifications = spec;
		}
		Invalidate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1 ,&m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}
	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			// when resizeBuffer we need to delete everything in Framebuffer Class
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}
		glCreateFramebuffers(1, &m_RendererID);
		// bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// if we have colorattchments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			// Attachments
			for (size_t i =0;i< m_ColorAttachments.size();++i)
			{
				auto foramt = m_ColorAttachmentSpecifications[i];
				Utils::CreateTextures(foramt.TextureFormat,multisample,&m_ColorAttachments[i], 1);
				Utils::BindTexture(foramt.TextureFormat,multisample,m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::Texture2D:
					Utils::AttachColorTexture(foramt.TextureFormat,foramt.DataFormat,m_ColorAttachments[i], m_Specification.Samples,m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::CubeMap:
					Utils::AttachColorTextureCube(foramt.DataFormat, m_ColorAttachments[i], m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}
		// if we have DepthAttachment
		if (m_DepthAttacmentSpecifications.TextureFormat != FramebufferTextureFormat::None)
		{
			auto foramt = m_DepthAttacmentSpecifications;
			Utils::CreateTextures(foramt.TextureFormat,multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(foramt.TextureFormat,multisample, m_DepthAttachment);
			switch (m_DepthAttacmentSpecifications.TextureFormat)
			{
			case FramebufferTextureFormat::Depth:
				Utils::AttachDepthTexture(foramt.TextureFormat,m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}
		if (m_ColorAttachments.size() > 1)
		{
			QCAT_CORE_ASSERT(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 ,GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if(m_ColorAttachments.empty())
		{
			// only Depth-pass
			glDrawBuffer(GL_NONE);
		}

		/*glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);*/

		QCAT_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFrameBuffer::Bind()
	{
		// bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		// ClearColorAttament(index,
	}
	void OpenGLFrameBuffer::UnBind()
	{
		// Unbind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width>s_MaxFramebufferSize||height>s_MaxFramebufferSize)
		{
			QCAT_CORE_WARN("Attempted to resize framebuffer to {0},{1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}
	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1,GL_RED_INTEGER,GL_INT,&pixelData);
		return pixelData;
	}
	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, const void* value)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::GetTextureFormat(spec.DataFormat), Utils::GetDataType(spec.DataFormat), value);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			QCAT_WARN("error code {1}", error);
	}
	void OpenGLFrameBuffer::AttachCubeMapByIndex(uint32_t faceindex)
	{
		QCAT_CORE_ASSERT(faceindex < 6);

		for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
		{
			if (m_ColorAttachmentSpecifications[i].TextureFormat == FramebufferTextureFormat::CubeMap)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 +i, GL_TEXTURE_CUBE_MAP_POSITIVE_X+ faceindex, m_ColorAttachments[i], 0);
			}
		}	
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			QCAT_CORE_ASSERT(false, "Framebuffer isnt Good");
	}
	void* OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());
		uint32_t id = m_ColorAttachments[index];
		return reinterpret_cast<void*>(id);
	}

	void OpenGLFrameBuffer::BindColorTexture(uint32_t slot, uint32_t index) const
	{
		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());

		glBindTextureUnit(slot, m_ColorAttachments[index]);
	}
}