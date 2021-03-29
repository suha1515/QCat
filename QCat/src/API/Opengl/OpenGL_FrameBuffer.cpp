#include "qcpch.h"
#include "OpenGL_FrameBuffer.h"

#include <glad/glad.h>

namespace QCat
{
	// Temporary code
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils{

		static GLenum GetDataType(TextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureDataFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case TextureDataFormat::RED32_INTEGER:  return GL_INT;
			case TextureDataFormat::RG16_Float:	    return GL_FLOAT;
			case TextureDataFormat::RGBA32_Float:	return GL_FLOAT;
			case TextureDataFormat::RGBA16_Float:	return GL_FLOAT;


				//24 bit
			case TextureDataFormat::RGB8:			return GL_UNSIGNED_BYTE;

			}
		}
		static GLenum GetTextureTarget(TextureType format,bool multisampled)
		{
			switch (format)
			{
			case TextureType::Texture2D:		  return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			case TextureType::TextureArray:       return GL_TEXTURE_2D_ARRAY;
			case TextureType::TextureCube:		  return GL_TEXTURE_CUBE_MAP;
			}
		}
		static GLenum GetTextureFormat(TextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureDataFormat::RGBA8:			 return GL_RGBA;
			case TextureDataFormat::RED32_INTEGER:   return GL_RED_INTEGER;
			case TextureDataFormat::RGBA32_Float:	 return GL_RGBA;
			case TextureDataFormat::RGBA16_Float:    return GL_RGBA;

				//24 bit
			case TextureDataFormat::RGB8:			 return GL_RGB;
			case TextureDataFormat::RG16_Float:	     return GL_RG;

				//depth
			case TextureDataFormat::DEPTH32:		 return GL_DEPTH_COMPONENT;
			case TextureDataFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
			}

		}
		static GLenum GetTextureInternalFormat(TextureDataFormat format)
		{
			switch (format)
			{
				
			//32 bit
			case TextureDataFormat::RGBA8:			return GL_RGBA8;
			case TextureDataFormat::RED32_INTEGER:  return GL_R32I;
			case TextureDataFormat::RG16_Float:	    return GL_RG16F;
			case TextureDataFormat::RGBA16_Float:   return GL_RGBA16F;
			case TextureDataFormat::RGBA32_Float:	return GL_RGBA32F;


			//24 bit
			case TextureDataFormat::RGB8:			return GL_RGB8;


			//depth
			case TextureDataFormat::DEPTH32:	     return GL_DEPTH_COMPONENT32;
			case TextureDataFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}
		}

		static GLenum GetTextureDataType(TextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureDataFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case TextureDataFormat::RED32_INTEGER:  return GL_INT;
			case TextureDataFormat::RG16_Float:	    return GL_FLOAT;
			case TextureDataFormat::RGBA16_Float:   return GL_FLOAT;
			case TextureDataFormat::RGBA32_Float:	return GL_FLOAT;

				//24 bit
			case TextureDataFormat::RGB8:			return GL_UNSIGNED_BYTE;


				//depth
			case TextureDataFormat::DEPTH32:		return GL_FLOAT;
			case TextureDataFormat::DEPTH24STENCIL8: return GL_UNSIGNED_INT_24_8;
			}
		}
	/*	static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}*/
		static void CreateTextures(TextureType format,bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(GetTextureTarget(format,multisampled), count, outID);
		}
		static void BindTexture(TextureType format,bool multisampled,uint32_t id)
		{
			glBindTexture(GetTextureTarget(format,multisampled), id);
		}
		static void AttachTexture(TextureType textureformat, TextureDataFormat dataformat,
			uint32_t id, int samples, uint32_t width, uint32_t height,GLenum attachmenIndex)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GetTextureInternalFormat(dataformat), width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(dataformat), width, height, 0, GetTextureFormat(dataformat), GetTextureDataType(dataformat), nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmenIndex, GetTextureTarget(textureformat,multisampled), id, 0);

		}
		static void AttachTextureCube(TextureType format, TextureDataFormat dataformat,uint32_t id, uint32_t width, uint32_t height,GLenum attachmentType)
		{
			for (int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetTextureInternalFormat(dataformat), width, height, 0, GetTextureFormat(dataformat), GetTextureDataType(dataformat), nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			glFramebufferTexture(GL_FRAMEBUFFER, attachmentType,id, 0);
		}
		static bool IsDepthFormat(FramebufferUsage format)
		{
			switch (format)
			{
			case FramebufferUsage::Depth: return true;
			case FramebufferUsage::Depth_Stencil: return true;
			}
			return false;
		}
		static GLenum QCatTextureFormatToGL(TextureDataFormat data)
		{
			switch (data)
			{
			case TextureDataFormat::RGBA8:			    return GL_RGBA;
			case TextureDataFormat::RED32_INTEGER:		return GL_RED_INTEGER;

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
			if (!Utils::IsDepthFormat(spec.usage))
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
				auto format = m_ColorAttachmentSpecifications[i];
				Utils::CreateTextures(format.textureType,multisample,&m_ColorAttachments[i], 1);
				Utils::BindTexture(format.textureType,multisample,m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].textureType)
				{
				case TextureType::Texture2D:
					Utils::AttachTexture(format.textureType, format.textureformat,m_ColorAttachments[i], m_Specification.Samples,m_Specification.Width, m_Specification.Height, GL_COLOR_ATTACHMENT0 +i);
					break;
				case TextureType::TextureCube:
					Utils::AttachTextureCube(format.textureType, format.textureformat, m_ColorAttachments[i], m_Specification.Width, m_Specification.Height,GL_COLOR_ATTACHMENT0 + i);
					break;
				}
			}
		}
		// if we have DepthAttachment
		if (m_DepthAttacmentSpecifications.usage != FramebufferUsage::None)
		{
			auto format = m_DepthAttacmentSpecifications;
			Utils::CreateTextures(format.textureType,multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(format.textureType,multisample, m_DepthAttachment);
			GLenum attachmentInfo;
			if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth)
				attachmentInfo = GL_DEPTH_ATTACHMENT;
			else if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth_Stencil)
				attachmentInfo = GL_DEPTH_STENCIL_ATTACHMENT;
			switch (m_DepthAttacmentSpecifications.textureType)
			{
			case TextureType::Texture2D:
				Utils::AttachTexture(format.textureType, format.textureformat, m_DepthAttachment, m_Specification.Samples, m_Specification.Width, m_Specification.Height,GL_DEPTH_ATTACHMENT);
				break;
			case TextureType::TextureCube:
				Utils::AttachTextureCube(format.textureType, format.textureformat, m_DepthAttachment, m_Specification.Width, m_Specification.Height, GL_DEPTH_ATTACHMENT);
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
			glReadBuffer(GL_NONE);
		}

		/*glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);*/

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Frambuffer is incomplete!");

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
	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y,int z)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y,1, 1,GL_RED_INTEGER,GL_INT,&pixelData);
		return pixelData;
	}
	void OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, TextureDataFormat format, void* value, int x, int y, int z)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, 1, 1, Utils::GetTextureFormat(format),Utils::GetTextureDataType(format), &value);
	}
	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, const void* value)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::GetTextureFormat(spec.textureformat), Utils::GetDataType(spec.textureformat), value);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			QCAT_WARN("error code {1}", error);
	}
	void OpenGLFrameBuffer::AttachCubeMapByIndex(uint32_t faceindex)
	{
		QCAT_CORE_ASSERT(faceindex < 6);

		for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
		{
			if (m_ColorAttachmentSpecifications[i].textureType == TextureType::TextureCube)
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
	void OpenGLFrameBuffer::BindDepthTexture(uint32_t slot) const
	{
		 glBindTextureUnit(slot, m_DepthAttachment); 
	}
	void OpenGLFrameBuffer::Clear(glm::vec4 color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
		if (m_ColorAttachments.size() > 0)
		{
			if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth_Stencil)
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			else if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth)
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			else
				glClear(GL_COLOR_BUFFER_BIT);
		}
		else if (m_ColorAttachments.size() == 0)
		{
			if(m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth)
				glClear(GL_DEPTH_BUFFER_BIT);
			else if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth_Stencil)
				glClear( GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
		}
			
	}
	void OpenGLFrameBuffer::CopyFrameBuffer(int srcx0, int srcy0, int srcx1, int srcy1, int dstx0, int dsty0, int dstx1, int dsty1, BufferBit bufferbit, void* destBuffer)
	{
		GLint Buffer=0;
		if (destBuffer != nullptr)
		 Buffer = (GLint)destBuffer;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Buffer);
		GLbitfield bit=0;	
		switch (bufferbit)
		{
		case BufferBit::Color:  bit = GL_COLOR_BUFFER_BIT; break;
		case BufferBit::Depth:  bit = GL_DEPTH_BUFFER_BIT; break; 
		case BufferBit::Stencil: bit = GL_STENCIL_BUFFER_BIT; break;
		}
		glBlitFramebuffer(srcx0, srcy0, srcx1, srcy1, dstx0, dsty0, dstx1, dsty1, bit, GL_NEAREST);
	}
}