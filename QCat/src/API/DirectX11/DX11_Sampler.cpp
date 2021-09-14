#include "qcpch.h"
#include "DX11_Sampler.h"

namespace QCat
{
	namespace Utils
	{
		D3D11_FILTER GetFilter(Filtering min, Filtering mag, Filtering mip,FilterMode mode)
		{
			if (mode == FilterMode::NONE)
			{
				switch (min)
				{
				case Filtering::POINT:
				{
					switch (mag)
					{
					case Filtering::POINT:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_MIN_MAG_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
						}
					}
					case Filtering::LINEAR:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
						}
					}
					}
				}
				case Filtering::LINEAR:
				{
					switch (mag)
					{
					case Filtering::POINT:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
						}
					}
					case Filtering::LINEAR:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
						}
					}
					}
				}
				case Filtering::ANISOTROPIC: return D3D11_FILTER_ANISOTROPIC;
				}
			}
			else if (mode == FilterMode::COMPARISON)
			{
				switch (min)
				{
				case Filtering::POINT:
				{
					switch (mag)
					{
					case Filtering::POINT:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
						}
					}
					case Filtering::LINEAR:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
						}
					}
					}
				}
				case Filtering::LINEAR:
				{
					switch (mag)
					{
					case Filtering::POINT:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
						}
					}
					case Filtering::LINEAR:
					{
						switch (mip)
						{
						case Filtering::POINT: return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
						case Filtering::NONE:
						case Filtering::LINEAR: return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
						}
					}
					}
				}
				case Filtering::ANISOTROPIC: return D3D11_FILTER_COMPARISON_ANISOTROPIC;
				}
			}
			else if (mode == FilterMode::MAXIMUM)
			{
			switch (min)
			{
			case Filtering::POINT:
			{
				switch (mag)
				{
				case Filtering::POINT:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
					}
				}
				case Filtering::LINEAR:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
					}
				}
				}
			}
			case Filtering::LINEAR:
			{
				switch (mag)
				{
				case Filtering::POINT:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
					}
				}
				case Filtering::LINEAR:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
					}
				}
				}
			}
			case Filtering::ANISOTROPIC: return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
			}
			}
			else if (mode == FilterMode::MINIMUM)
			{
			switch (min)
			{
			case Filtering::POINT:
			{
				switch (mag)
				{
				case Filtering::POINT:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
					}
				}
				case Filtering::LINEAR:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
					}
				}
				}
			}
			case Filtering::LINEAR:
			{
				switch (mag)
				{
				case Filtering::POINT:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
					}
				}
				case Filtering::LINEAR:
				{
					switch (mip)
					{
					case Filtering::POINT: return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
					case Filtering::NONE:
					case Filtering::LINEAR: return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
					}
				}
				}
			}
			case Filtering::ANISOTROPIC: return D3D11_FILTER_MINIMUM_ANISOTROPIC;
			}
			}	
		}

		D3D11_TEXTURE_ADDRESS_MODE GetAddressMode(WrapingMode mode)
		{
			switch (mode)
			{
			case WrapingMode::REPEAT: return D3D11_TEXTURE_ADDRESS_WRAP;
			case WrapingMode::MIRROR: return D3D11_TEXTURE_ADDRESS_MIRROR;
			case WrapingMode::MIRROR_ONCE: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
			case WrapingMode::CLAMP: return D3D11_TEXTURE_ADDRESS_CLAMP;
			case WrapingMode::BORDER: return D3D11_TEXTURE_ADDRESS_BORDER;
			}
		}
		D3D11_COMPARISON_FUNC ComparisonToDx(COMPARISON_FUNC func)
		{
			switch (func)
			{
			case COMPARISON_FUNC::ALWAYS:			return D3D11_COMPARISON_ALWAYS;
			case COMPARISON_FUNC::GREATER:		return D3D11_COMPARISON_GREATER;
			case COMPARISON_FUNC::GREATER_EQUAL:  return D3D11_COMPARISON_GREATER_EQUAL;
			case COMPARISON_FUNC::LESS:			return D3D11_COMPARISON_LESS;
			case COMPARISON_FUNC::LESS_EQUAL:		return D3D11_COMPARISON_LESS_EQUAL;
			case COMPARISON_FUNC::NOT_EQUAL:		return D3D11_COMPARISON_NOT_EQUAL;
			case COMPARISON_FUNC::EQUAL:			return D3D11_COMPARISON_EQUAL;
			case COMPARISON_FUNC::NEVER:			return D3D11_COMPARISON_NEVER;
			}
			QCAT_CORE_ASSERT(false, "Wrong ComparisonFunc value.");

		}
	}
	DX11Sampler::DX11Sampler(Sampler_Desc desc)
	{
		this->desc = desc;
		Invalidate();
	}
	DX11Sampler::~DX11Sampler()
	{
	}
	void DX11Sampler::Bind(unsigned int slot, ShaderType type)
	{
		if (IsChanged)
		{
			Invalidate();
			IsChanged = false;
		}
		switch (type)
		{
		case ShaderType::VS:
			QGfxDeviceDX11::GetInstance()->GetContext()->VSSetSamplers(slot, 1u, pSamplerState.GetAddressOf()); break;
		case ShaderType::GS:
			QGfxDeviceDX11::GetInstance()->GetContext()->GSSetSamplers(slot, 1u, pSamplerState.GetAddressOf()); break;
		case ShaderType::PS:
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetSamplers(slot, 1u, pSamplerState.GetAddressOf()); break;
		case ShaderType::CS:
			QGfxDeviceDX11::GetInstance()->GetContext()->CSSetSamplers(slot, 1u, pSamplerState.GetAddressOf()); break;
		}
	}
	void DX11Sampler::UnBind(unsigned int slot, ShaderType type )
	{
		ID3D11SamplerState* nullSampler[1] = { 0 };
		switch (type)
		{
		case ShaderType::VS:
			QGfxDeviceDX11::GetInstance()->GetContext()->VSSetSamplers(slot, 1u, nullSampler); break;
		case ShaderType::GS:
			QGfxDeviceDX11::GetInstance()->GetContext()->GSSetSamplers(slot, 1u, nullSampler); break;
		case ShaderType::PS:
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetSamplers(slot, 1u, nullSampler); break;
		case ShaderType::CS:
			QGfxDeviceDX11::GetInstance()->GetContext()->CSSetSamplers(slot, 1u, nullSampler); break;
		}
	}
	void DX11Sampler::Invalidate()
	{
		if (pSamplerState)
			pSamplerState.Reset();

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

		samplerDesc.Filter = Utils::GetFilter(desc.MIN, desc.MAG, desc.MIP, desc.mode);
		samplerDesc.AddressU = Utils::GetAddressMode(desc.addressU);
		samplerDesc.AddressV = Utils::GetAddressMode(desc.addressV);
		samplerDesc.AddressW = Utils::GetAddressMode(desc.addressW);

		samplerDesc.BorderColor[0] = desc.borderColor[0];
		samplerDesc.BorderColor[1] = desc.borderColor[1];
		samplerDesc.BorderColor[2] = desc.borderColor[2];
		samplerDesc.BorderColor[3] = desc.borderColor[3];

		samplerDesc.ComparisonFunc = Utils::ComparisonToDx(desc.comparisonFunc);
		samplerDesc.MaxAnisotropy = desc.maxAnisotropy;
		samplerDesc.MaxLOD = desc.maxLod;
		samplerDesc.MinLOD = desc.minLod;
		samplerDesc.MipLODBias = desc.MipLodBias;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState);
	}
}
