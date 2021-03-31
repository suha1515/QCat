#include "qcpch.h"
#include "OpenGL_FrameBuffer.h"
#include "API/Opengl/OpenglUtils.h"

namespace QCat
{
	// Temporary code
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils{
	
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
		static void AttachTexture(TextureType textureformat,uint32_t id,bool multisampled, GLenum attachmenIndex)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmenIndex, GetTextureTarget(textureformat,multisampled), id, 0);
		}
		static void AttachTextureCube(TextureType format, uint32_t id,GLenum attachmentType)
		{
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
		static GLenum QCatTextureFormatToGL(TextureFormat data)
		{
			switch (data)
			{
			case TextureFormat::RGBA8:			    return GL_RGBA;
			case TextureFormat::RED32_INTEGER:		return GL_RED_INTEGER;

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
	}
	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			// when resizeBuffer we need to delete everything in Framebuffer Class
			glDeleteFramebuffers(1, &m_RendererID);
			m_ColorAttachments.clear();
			m_DepthAttachment.reset();
		}
		glCreateFramebuffers(1, &m_RendererID);
		// bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// if we have colorattchments
		if (m_ColorAttachmentSpecifications.size())
		{
			// Attachments
			for (size_t i =0;i< m_ColorAttachmentSpecifications.size();++i)
			{
				auto format = m_ColorAttachmentSpecifications[i];
				switch (m_ColorAttachmentSpecifications[i].textureType)
				{
				case TextureType::Texture2D:
				{
					Sampler_Desc desc;
					Ref<Texture2D> texture = Texture2D::Create(format.textureformat, desc, m_Specification.Width, m_Specification.Height, 1, m_Specification.Samples);
					GLint id = (GLint)texture->GetTexture();
					Utils::BindTexture(format.textureType, multisample, id);
					Utils::AttachTexture(format.textureType, id, multisample, GL_COLOR_ATTACHMENT0 + i);
					m_ColorAttachments.push_back(texture);
				}
				break;
				case TextureType::TextureCube:
				{
					Sampler_Desc desc;
					Ref<TextureCube> textureCube = TextureCube::Create(format.textureformat, desc, m_Specification.Width, m_Specification.Height);
					GLint id = (GLint)textureCube->GetTexture();
					Utils::BindTexture(format.textureType, multisample, id);
					Utils::AttachTextureCube(format.textureType, id, GL_COLOR_ATTACHMENT0 + i);
					m_ColorAttachments.push_back(textureCube);
				}
					break;
				}
			}
		}
		// if we have DepthAttachment
		if (m_DepthAttacmentSpecifications.usage != FramebufferUsage::None)
		{
			auto format = m_DepthAttacmentSpecifications;
			
			GLenum attachmentInfo;
			if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth)
				attachmentInfo = GL_DEPTH_ATTACHMENT;
			else if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth_Stencil)
				attachmentInfo = GL_DEPTH_STENCIL_ATTACHMENT;
			switch (m_DepthAttacmentSpecifications.textureType)
			{
			case TextureType::Texture2D:
			{
				Sampler_Desc desc;
				m_DepthAttachment = Texture2D::Create(format.textureformat,desc, m_Specification.Width, m_Specification.Height, 1, m_Specification.Samples);
				GLint id = (GLint)m_DepthAttachment->GetTexture();
				Utils::BindTexture(format.textureType, multisample, id);
				Utils::AttachTexture(format.textureType, id, multisample, attachmentInfo);
			}
				break;
			case TextureType::TextureCube:
			{
				Sampler_Desc desc;
				m_DepthAttachment = TextureCube::Create(format.textureformat,desc, m_Specification.Width, m_Specification.Height);
				GLint id = (GLint)m_DepthAttachment->GetTexture();
				Utils::BindTexture(format.textureType, multisample, id);
				Utils::AttachTexture(format.textureType, id, multisample, attachmentInfo);
			}
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
	void OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, TextureFormat format, void* value, int x, int y, int z)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, 1, 1, Utils::GetTextureFormat(format),Utils::GetTextureDataFormat(format), &value);
	}
	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, const void* value)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage((GLint)m_ColorAttachments[attachmentIndex]->GetTexture(), 0, Utils::GetTextureFormat(spec.textureformat), Utils::GetOpengLDataType(spec.textureformat), value);
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
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 +i, GL_TEXTURE_CUBE_MAP_POSITIVE_X+ faceindex, (GLint)m_ColorAttachments[i]->GetTexture(), 0);
			}
		}	
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			QCAT_CORE_ASSERT(false, "Framebuffer isnt Good");
	}
	void* OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());
		return m_ColorAttachments[index]->GetTexture();
	}

	void OpenGLFrameBuffer::BindColorTexture(uint32_t slot, uint32_t index) const
	{
		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());

		m_ColorAttachments[index]->Bind(slot);
	}
	void OpenGLFrameBuffer::BindDepthTexture(uint32_t slot) const
	{
		m_DepthAttachment->Bind(slot);
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