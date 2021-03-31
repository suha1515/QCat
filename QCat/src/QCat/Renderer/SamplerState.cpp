#include "qcpch.h"
#include "SamplerState.h"
#include "RenderAPI.h"

#include "API/Opengl/Opengl_Sampler.h"
#include "API/DirectX11/DX11_Sampler.h"

namespace QCat
{
    Ref<SamplerState> QCat::SamplerState::Create(Sampler_Desc desc)
    {
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: QCAT_CORE_ASSERT(false, "DepthStencilState need RenderAPI information"); return nullptr;
		case RenderAPI::API::OpenGL: return CreateRef<OpenGLSampler>(desc);
		case RenderAPI::API::DirectX11: return CreateRef<DX11Sampler>(desc);
		}
		QCAT_CORE_ASSERT(false, "Error! Creating SamplerState");
		return nullptr;
    }
	void SamplerState::SetMinFilter(Filtering filtering)
	{
		if (desc.MIN != filtering)
		{
			desc.MIN = filtering;
			IsChanged = true;
		}
	}
	void SamplerState::SetMagFilter(Filtering filtering)
	{
		if (desc.MAG != filtering)
		{
			desc.MAG = filtering;
			IsChanged = true;
		}
	}
	void SamplerState::SetMipFilter(Filtering filtering)
	{
		if (desc.MIP != filtering)
		{
			desc.MIP = filtering;
			IsChanged = true;
		}
	}
	void SamplerState::SetFilterMode(FilterMode mode)
	{
		if (desc.mode != mode)
		{
			desc.mode = mode;
			IsChanged = true;
		}
	}
	void SamplerState::SetTextureWrapperU(WrapingMode wrapmode)
	{
		if (desc.addressU != wrapmode)
		{
			desc.addressU = wrapmode;
			IsChanged = true;
		}
	}
	void SamplerState::SetTextureWrapperV(WrapingMode wrapmode)
	{
		if (desc.addressV != wrapmode)
		{
			desc.addressV = wrapmode;
			IsChanged = true;
		}
	}
	void SamplerState::SetTextureWrapperW(WrapingMode wrapmode)
	{
		if (desc.addressW != wrapmode)
		{
			desc.addressW = wrapmode;
			IsChanged = true;
		}
	}
	void SamplerState::SetMipLodBias(float bias)
	{
		if (desc.MipLodBias != bias)
		{
			desc.MipLodBias = bias;
			IsChanged = true;
		}
	}
	void SamplerState::SetMaxAnisotropy(unsigned int max)
	{
		if (desc.maxAnisotropy != max)
		{
			desc.maxAnisotropy = max;
			IsChanged = true;
		}
	}
	void SamplerState::SetComparisonFunc(COMPARISON_FUNC func)
	{
		if (desc.comparisonFunc != func)
		{
			desc.comparisonFunc = func;
			IsChanged = true;
		}
	}
	void SamplerState::SetBorderColor(float* borderColor)
	{
		if (desc.borderColor[0] != borderColor[0] || desc.borderColor[1] != borderColor[1]||
			desc.borderColor[2] != borderColor[2] || desc.borderColor[3] != borderColor[3])
		{
			desc.borderColor[0] = borderColor[0];
			desc.borderColor[1] = borderColor[1];
			desc.borderColor[2] = borderColor[2];
			desc.borderColor[3] = borderColor[3];
			IsChanged = true;
		}
	}
	void SamplerState::SetMinLod(float minLod)
	{
		if (desc.minLod != minLod)
		{
			desc.minLod = minLod;
			IsChanged = true;
		}
	}
	void SamplerState::SetMaxLod(float maxLod)
	{
		if (desc.maxLod != maxLod)
		{
			desc.maxLod = maxLod;
			IsChanged = true;
		}
	}
}

