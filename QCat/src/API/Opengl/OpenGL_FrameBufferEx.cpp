#include "qcpch.h"
#include "OpenGL_FrameBufferEx.h"

namespace QCat
{
	OpenGLFrameBufferEx::OpenGLFrameBufferEx(const AttachmentSpecification& attachments)
		:m_AttachmentSpecifications(attachments.Attachments)
	{
		glCreateFramebuffers(1, &m_RendererID);
		uint32_t ColorBufferCount = 0;
		uint32_t DepthBufferCount = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		for(auto spec : m_AttachmentSpecifications)
		{
			if (spec.usage != FramebufferUsage::Depth && spec.usage != FramebufferUsage::Depth_Stencil)
			{
				ColorBufferCount++;
				colorbit = true;
			}
			else if (spec.usage == FramebufferUsage::Depth)
			{
				DepthBufferCount++;
				depthbit = true;
			}
			else if (spec.usage == FramebufferUsage::Depth_Stencil)
			{
				DepthBufferCount++;
				stencilbit = true;
				depthbit = true;
			}			
		}

		if (ColorBufferCount > 1)
		{
			std::vector<GLenum> buffers;
			for (int i = 0; i < ColorBufferCount; ++i)
			{
				buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			}
			glDrawBuffers(ColorBufferCount, buffers.data());
		}
		else if (ColorBufferCount == 0)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	OpenGLFrameBufferEx::~OpenGLFrameBufferEx()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}
	void OpenGLFrameBufferEx::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}
	void OpenGLFrameBufferEx::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBufferEx::InitializeTexture(const std::string& name, const Texture_Desc& texDesc, const Sampler_Desc& samplerDesc)
	{
		TextureFormat format = TextureFormat::None;
		TextureType type = TextureType::None;
		FramebufferUsage usage;
		for (auto& spec : m_AttachmentSpecifications)
		{
			if (spec.name == name)
			{
				format = spec.textureformat;
				type = spec.textureType;
				usage = spec.usage;
				break;
			}
		}
		if (format == TextureFormat::None)
		{
			QCAT_CORE_ERROR("FrameBuffer Init Texture Error : there are no specification for name ");
			//QCAT_CORE_ASSERT(false)
			//std::runtime_error("FrameBuffer Init Texture Error : there are no specification for name");
		}
		Ref<Texture> texture;
		switch (type)
		{
		case TextureType::None:
			QCAT_CORE_ERROR("FrameBuffer Init Texture Error : There are nono TextureType info");
			break;
		case TextureType::Texture2D:
			texture = Texture2D::Create(format,samplerDesc, texDesc.Width, texDesc.Height, texDesc.MipLevels, texDesc.SampleCount);
			break;
		case TextureType::TextureArray:
			break;
		case TextureType::TextureCube:
			texture = TextureCube::Create(format, samplerDesc, texDesc.Width, texDesc.Height, texDesc.MipLevels);
			break;
		}

		auto find_iter = m_Textures.find(name);
		if (find_iter != m_Textures.end())
			m_Textures[name] = texture;
		else
			m_Textures.emplace(name, texture);
	}
	
	namespace Utils
	{	
		GLenum GetAttachmentType(AttachmentType type)
		{
			switch (type)
			{
				case AttachmentType::Color_0: return GL_COLOR_ATTACHMENT0;
				case AttachmentType::Color_1: return GL_COLOR_ATTACHMENT1;
				case AttachmentType::Color_2: return GL_COLOR_ATTACHMENT2;
				case AttachmentType::Color_3: return GL_COLOR_ATTACHMENT3;
				case AttachmentType::Color_4: return GL_COLOR_ATTACHMENT4;
				case AttachmentType::Depth:	  return GL_DEPTH_ATTACHMENT;
				case AttachmentType::Depth_Stencil: return GL_DEPTH_STENCIL_ATTACHMENT;
			}
		}
		GLenum GetTexTarget(TextureType type,bool multisampled)
		{
			switch (type)
			{
				case TextureType::Texture2D:			 return multisampled ? GL_TEXTURE_2D_MULTISAMPLE :GL_TEXTURE_2D;
				case TextureType::TextureArray:			 return multisampled ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
				case TextureType::TextureCube:			 return GL_TEXTURE_CUBE_MAP;
				case TextureType::TextureCube_PositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				case TextureType::TextureCube_NegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
				case TextureType::TextureCube_PositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
				case TextureType::TextureCube_NegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
				case TextureType::TextureCube_PositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
				case TextureType::TextureCube_NegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			}
		}
		void BindTexture(TextureType format, bool multisampled, uint32_t id)
		{
			glBindTexture(GetTexTarget(format, multisampled), id);
		}
	}
	void OpenGLFrameBufferEx::AttachTexture(const std::string& name, AttachmentType attachType, TextureType type, uint32_t mipLevel)
	{
		Ref<Texture> texture = m_Textures[name];
		if (texture != nullptr)
		{
			bool multisampled = texture->GetDesc().MipLevels > 1;
			GLenum attachmentIndex = Utils::GetAttachmentType(attachType);
			GLint texId = (GLint)texture->GetTexture();
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentIndex, Utils::GetTexTarget(type,multisampled), texId, mipLevel);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is bad");
		}
		else
		{
			QCAT_CORE_ERROR("There are no Texture in Framebuffer , name : {0}", name);
		}	
	}

	void OpenGLFrameBufferEx::AttachTexture(const Ref<Texture>& texture, AttachmentType attachType, TextureType type, uint32_t mipLevel)
	{
		if (texture != nullptr)
		{
			bool multisampled = texture->GetDesc().SampleCount > 1;
			GLenum attachmentIndex = Utils::GetAttachmentType(attachType);
			GLint texId = (GLint)texture->GetTexture();
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentIndex, Utils::GetTexTarget(type, multisampled), texId, mipLevel);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is bad");
		}
		else
		{
			QCAT_CORE_ERROR("FrameBuffer attach error texture is nullptr");
		}
	}

	void OpenGLFrameBufferEx::DetachTexture(AttachmentType attachType)
	{
		GLenum attachmentIndex = Utils::GetAttachmentType(attachType);
		glFramebufferTexture(GL_FRAMEBUFFER, attachmentIndex,0,0);
	}

	void OpenGLFrameBufferEx::DetachAll()
	{
		unsigned int colorCount = 0;
		for (auto& sepc : m_AttachmentSpecifications)
		{
			if (sepc.usage == FramebufferUsage::Depth_Stencil)
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
			else if (sepc.usage == FramebufferUsage::Depth)
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
			else
				colorCount++;
		}

		for(int i=0;i<colorCount;++i)
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
	}

	Ref<Texture> OpenGLFrameBufferEx::GetTexture(const std::string& name)
	{
		auto find_iter = m_Textures.find(name);
		if (find_iter != m_Textures.end())
			return find_iter->second;
		else
			return nullptr;
	}

	void OpenGLFrameBufferEx::Clear(const glm::vec4 color)
	{
		GLbitfield mask=0;
		if (colorbit)
			mask |= GL_COLOR_BUFFER_BIT;
		if (depthbit)
			mask |= GL_DEPTH_BUFFER_BIT;
		if (stencilbit)
			mask |= GL_STENCIL_BUFFER_BIT;

		glClear(mask);
	}
}