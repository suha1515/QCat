#pragma once
#include <numeric>
#include "QCat/Renderer/enums.h"
namespace QCat
{
	struct Sampler_Desc
	{
		Sampler_Desc() = default;
		Sampler_Desc(Filtering min,Filtering mag, Filtering mip,
					 WrapingMode U, WrapingMode V, WrapingMode W,float* borderColor,
				     FilterMode mode,COMPARISON_FUNC func,unsigned int maxAnisotropy,float minLod,float maxLod, float mipLodbias)
			:MIN(min),MAG(mag),MIP(mip),addressU(U),addressV(V),addressW(W),mode(mode),comparisonFunc(func)
			,maxAnisotropy(maxAnisotropy),minLod(minLod),maxLod(maxLod),MipLodBias(mipLodbias)
		{
			this->borderColor[0] = borderColor[0];
			this->borderColor[1] = borderColor[1];
			this->borderColor[2] = borderColor[2];
			this->borderColor[3] = borderColor[3];
		}

		Filtering MIN = Filtering::LINEAR;
		Filtering MAG = Filtering::LINEAR;
		Filtering MIP = Filtering::LINEAR;
		FilterMode mode = FilterMode::NONE;

		WrapingMode addressU = WrapingMode::CLAMP;
		WrapingMode addressV = WrapingMode::CLAMP;
		WrapingMode addressW = WrapingMode::CLAMP;

		float MipLodBias = 0.0f;
		unsigned int maxAnisotropy = 1;

		COMPARISON_FUNC comparisonFunc = COMPARISON_FUNC::NEVER;
		float borderColor[4] = { 1.0f,1.0f,1.0f,1.0f };

		float minLod = -std::numeric_limits<float>::max();
		float maxLod = std::numeric_limits<float>::max();
	};
	class SamplerState
	{
	public:
		SamplerState() = default;
		SamplerState(Sampler_Desc desc):desc(desc)
		{}
		virtual ~SamplerState() = default;

		virtual void Bind(unsigned int slot) = 0;
		virtual void UnBind(unsigned int slot) = 0;

		virtual void Invalidate() = 0;

		void SetMinFilter(Filtering filtering);
		void SetMagFilter(Filtering filtering);
		void SetMipFilter(Filtering filtering);
		void SetFilterMode(FilterMode mode);

		void SetTextureWrapperU(WrapingMode wrapmode);
		void SetTextureWrapperV(WrapingMode wrapmode);
		void SetTextureWrapperW(WrapingMode wrapmode);

		void SetMipLodBias(float bias);
		void SetMaxAnisotropy(unsigned int max);
		void SetComparisonFunc(COMPARISON_FUNC func);
		void SetBorderColor(float* borderColor);
		void SetMinLod(float minLod);
		void SetMaxLod(float maxLod);

		static Ref<SamplerState> Create(Sampler_Desc desc);
	protected:
		Sampler_Desc desc;
		bool IsChanged = false;
	};
}