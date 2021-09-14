#pragma once
#include "QCat/Renderer/SamplerState.h"

namespace QCat
{
	class OpenGLSampler : public SamplerState
	{
	public:
		OpenGLSampler(Sampler_Desc desc);
		virtual ~OpenGLSampler();

		virtual void Bind(unsigned int slot, ShaderType type = ShaderType::PS)override;
		virtual void UnBind(unsigned int slot, ShaderType type = ShaderType::PS) override;
	
		void Invalidate() override;
	private:
		unsigned int id=0;
	};
}