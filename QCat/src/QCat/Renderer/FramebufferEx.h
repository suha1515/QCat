#pragma once
#include "QCat/Core/QCatCore.h"
#include "Texture.h"
#include "enums.h"

namespace QCat
{
	struct AttachmentInfo
	{
		AttachmentInfo() = default;
		AttachmentInfo(FramebufferUsage usage, TextureType textureType, TextureFormat dataFormat,std::string name)
			: usage(usage), textureType(textureType), textureformat(dataFormat),name(name) {}

	public:
		FramebufferUsage usage = FramebufferUsage::None;
		TextureType textureType = TextureType::None;
		TextureFormat textureformat = TextureFormat::None;
		std::string name;
	};

	struct AttachmentSpecification
	{
		AttachmentSpecification() = default;
		AttachmentSpecification(std::initializer_list<AttachmentInfo> attachments)
			:Attachments(attachments) {}

		std::vector< AttachmentInfo> Attachments;
		std::uint32_t Width=0;
		std::uint32_t Height =0;
	};
	class FrameBufferEx
	{
	public:
		FrameBufferEx() = default;
		virtual ~FrameBufferEx() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		
		virtual void InitializeTexture(const std::string& name,const Texture_Desc& texDesc,const Sampler_Desc& samplerDesc)=0;
		virtual void AttachTexture(const std::string& name, AttachmentType attachType, TextureType type, uint32_t mipLevel) = 0;
		virtual void AttachTexture(const Ref<Texture>& texture, AttachmentType attachType, TextureType type, uint32_t mipLevel)=0;

		virtual void AttachColorTexture(const Ref<RenderTargetView>& textureView, AttachmentType type)=0;
		virtual void AttachDepthTexture(const Ref<DepthStencilView>& textureView, AttachmentType type) = 0;

		virtual void DetachTexture(AttachmentType attachType)=0;
		virtual void DetachAll()=0;

		virtual Ref<Texture> GetTexture(const std::string& name) = 0;

		virtual void Clear(const glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f })=0;

		virtual const AttachmentSpecification& GetSpecification() const = 0;
		static Ref<FrameBufferEx> Create(AttachmentSpecification attachments);
	};
}