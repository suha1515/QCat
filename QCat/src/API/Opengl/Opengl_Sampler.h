#pragma once
#include "QCat/Renderer/SamplerState.h"

namespace QCat
{
	class OpenGLSampler : public SamplerState
	{
	public:
		OpenGLSampler(Sampler_Desc desc);
		virtual ~OpenGLSampler();

		virtual void Bind(unsigned int slot)override;
		virtual void UnBind(unsigned int slot) override;
	
		void Invalidate();
	private:
		unsigned int id=0;
	};
}