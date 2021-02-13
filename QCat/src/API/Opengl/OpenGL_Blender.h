#pragma once
#include "QCat/Renderer/Blend.h"

namespace QCat
{
	class OpenGLBlender : public Blender
	{
	public:
		OpenGLBlender();
		virtual ~OpenGLBlender()=default;

		virtual void Bind()const override {};
		virtual void UnBind()const override {};

		virtual void SetEnableBlend(uint32_t index, bool enable) override;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) override;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) override;
		virtual void SetIndependentBlend(bool enable) override;
		virtual void SetAlphaToCoverage(bool enable) override;
	private:

	};
}