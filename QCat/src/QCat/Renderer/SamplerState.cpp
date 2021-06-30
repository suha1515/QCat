#include "qcpch.h"
#include "SamplerState.h"
#include "RenderAPI.h"

#include "API/Opengl/Opengl_Sampler.h"
#include "API/DirectX11/DX11_Sampler.h"
#include <sstream>

namespace QCat
{
	std::string SamplerState::GetSignature()
	{
		std::string signature = "Filtering_MIN_";
		if (desc.MIN == Filtering::NONE)
		{
			signature += "NONE_";
		}
		else if (desc.MIN == Filtering::POINT)
		{
			signature += "POINT_";
		}
		else if (desc.MIN == Filtering::LINEAR)
		{
			signature += "LINEAR_";
		}
		else if (desc.MIN == Filtering::ANISOTROPIC)
		{
			signature += "ANISOTROPIC_";
		}
		signature += "MAG_";
		if (desc.MAG == Filtering::NONE)
		{
			signature += "NONE_";
		}
		else if (desc.MAG == Filtering::POINT)
		{
			signature += "POINT_";
		}
		else if (desc.MAG == Filtering::LINEAR)
		{
			signature += "LINEAR_";
		}
		else if (desc.MAG == Filtering::ANISOTROPIC)
		{
			signature += "ANISOTROPIC_";
		}
		signature += "MIP_";
		if (desc.MIP == Filtering::NONE)
		{
			signature += "NONE_";
		}
		else if (desc.MIP == Filtering::POINT)
		{
			signature += "POINT_";
		}
		else if (desc.MIP == Filtering::LINEAR)
		{
			signature += "LINEAR_";
		}
		else if (desc.MIP == Filtering::ANISOTROPIC)
		{
			signature += "ANISOTROPIC_";
		}

		signature += "FilterMode_";
		if (desc.mode == FilterMode::NONE)
		{
			signature += "NONE_";
		}
		else if (desc.mode == FilterMode::MINIMUM)
		{
			signature += "MINIMUM_";
		}
		else if (desc.mode == FilterMode::MAXIMUM)
		{
			signature += "MAXIMUM_";
		}
		else if (desc.mode == FilterMode::COMPARISON)
		{
			signature += "COMPARISION_";
		}

		signature += "Adress_U_";
		if (desc.addressU == WrapingMode::REPEAT)
		{
			signature += "REPEAT_";
		}
		else if (desc.addressU == WrapingMode::MIRROR_ONCE)
		{
			signature += "MIRRORONCE_";
		}
		else if (desc.addressU == WrapingMode::MIRROR)
		{
			signature += "MIRROR_";
		}
		else if (desc.addressU == WrapingMode::CLAMP)
		{
			signature += "CLAMP_";
		}
		else if (desc.addressU == WrapingMode::BORDER)
		{
			signature += "BORDER_";
		}

		signature += "V_";
		if (desc.addressV == WrapingMode::REPEAT)
		{
			signature += "REPEAT_";
		}
		else if (desc.addressV == WrapingMode::MIRROR_ONCE)
		{
			signature += "MIRRORONCE_";
		}
		else if (desc.addressV == WrapingMode::MIRROR)
		{
			signature += "MIRROR_";
		}
		else if (desc.addressV == WrapingMode::CLAMP)
		{
			signature += "CLAMP_";
		}
		else if (desc.addressV == WrapingMode::BORDER)
		{
			signature += "BORDER_";
		}
		signature += "W_";
		if (desc.addressW == WrapingMode::REPEAT)
		{
			signature += "REPEAT_";
		}
		else if (desc.addressW == WrapingMode::MIRROR_ONCE)
		{
			signature += "MIRRORONCE_";
		}
		else if (desc.addressW == WrapingMode::MIRROR)
		{
			signature += "MIRROR_";
		}
		else if (desc.addressW == WrapingMode::CLAMP)
		{
			signature += "CLAMP_";
		}
		else if (desc.addressW == WrapingMode::BORDER)
		{
			signature += "BORDER_";
		}
		
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << desc.MipLodBias;
		std::string value = stream.str();
		signature += "MipLodBias_"+value+"_";

		signature += "MaxAnisotropy_" + std::to_string(desc.maxAnisotropy) + "_";

		signature += "ComparisonFunc_";
		if (desc.comparisonFunc == COMPARISON_FUNC::ALWAYS)
		{
			signature += "ALWAYS_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::EQUAL)
		{
			signature += "EQUAL_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::GREATER)
		{
			signature += "GREATER_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::GREATER_EQUAL)
		{
			signature += "GREATEREQUAL_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::LESS)
		{
			signature += "LESS_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::LESS_EQUAL)
		{
			signature += "LESSEQUAL_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::NEVER)
		{
			signature += "NEVER_";
		}
		else if (desc.comparisonFunc == COMPARISON_FUNC::NOT_EQUAL)
		{
			signature += "NOTEQUAL_";
		}
		signature += "BorderColor_";
		stream.str("");
		stream << desc.borderColor[0] <<","<< desc.borderColor[1] << "," << desc.borderColor[2]<<"," << desc.borderColor[3];
		value = stream.str();
		signature += value + "_";

		signature += "MinMaxLod_";
		stream.str("");
		stream << desc.minLod<<","<<desc.maxLod;
		value = stream.str();
		signature += value;

		return signature;
	}
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
	Sampler_Desc SamplerState::MakeSamplerDesc(const std::string& signature)
	{
		Sampler_Desc desc;
		std::stringstream stream(signature);

		std::string value;
		std::getline(stream, value, '_');
		if (value == "Filtering")
		{
			std::getline(stream, value, '_');
			if (value == "MIN")
			{
				std::getline(stream, value, '_');
				if (value == "NONE")
				{
					desc.MIN = Filtering::NONE;
				}
				else if (value == "POINT")
				{
					desc.MIN = Filtering::POINT;
				}
				else if (value == "LINEAR")
				{
					desc.MIN = Filtering::LINEAR;
				}
				else if (value == "ANISOTROPIC")
				{
					desc.MIN = Filtering::ANISOTROPIC;
				}
			}
			std::getline(stream, value, '_');
			if (value == "MAG")
			{
				std::getline(stream, value, '_');
				if (value == "NONE")
				{
					desc.MAG = Filtering::NONE;
				}
				else if (value == "POINT")
				{
					desc.MAG = Filtering::POINT;
				}
				else if (value == "LINEAR")
				{
					desc.MAG = Filtering::LINEAR;
				}
				else if (value == "ANISOTROPIC")
				{
					desc.MAG = Filtering::ANISOTROPIC;
				}
			}
			std::getline(stream, value, '_');
			if (value == "MIP")
			{
				std::getline(stream, value, '_');
				if (value == "NONE")
				{
					desc.MIP = Filtering::NONE;
				}
				else if (value == "POINT")
				{
					desc.MIP = Filtering::POINT;
				}
				else if (value == "LINEAR")
				{
					desc.MIP = Filtering::LINEAR;
				}
				else if (value == "ANISOTROPIC")
				{
					desc.MIP = Filtering::ANISOTROPIC;
				}
			}
		}
		else
		{
			QCAT_CORE_ASSERT(false, "Filtering doesnt come First");
		}
		
		std::getline(stream, value, '_');
		if (value == "FilterMode")
		{
			std::getline(stream, value, '_');
			if (value == "NONE")
				desc.mode = FilterMode::NONE;
			else if(value == "COMPARISON")
				desc.mode = FilterMode::COMPARISON;
			else if (value == "MINIMUM")
				desc.mode = FilterMode::MINIMUM;
			else if (value == "MAXIMUM")
				desc.mode = FilterMode::MAXIMUM;
		}
		else
			QCAT_CORE_ASSERT(false, "FilterMode doesnt come Second!");
		std::getline(stream, value, '_');
		if (value == "Adress")
		{
			std::getline(stream, value, '_');
			std::getline(stream, value, '_');
				if (value == "REPEAT")
					desc.addressU = WrapingMode::REPEAT;
				else if(value =="MIRROR")
					desc.addressU = WrapingMode::MIRROR;
				else if (value == "CLAMP")
					desc.addressU = WrapingMode::CLAMP;
				else if (value == "BORDER")
					desc.addressU = WrapingMode::BORDER;
				else if (value == "MIRRORONCE")
					desc.addressU = WrapingMode::MIRROR_ONCE;
			std::getline(stream, value, '_');
			std::getline(stream, value, '_');
				if (value == "REPEAT")
					desc.addressV = WrapingMode::REPEAT;
				else if (value == "MIRROR")
					desc.addressV = WrapingMode::MIRROR;
				else if (value == "CLAMP")
					desc.addressV = WrapingMode::CLAMP;
				else if (value == "BORDER")
					desc.addressV = WrapingMode::BORDER;
				else if (value == "MIRRORONCE")
					desc.addressV = WrapingMode::MIRROR_ONCE;
			std::getline(stream, value, '_');
			std::getline(stream, value, '_');
				if (value == "REPEAT")
					desc.addressW = WrapingMode::REPEAT;
				else if (value == "MIRROR")
					desc.addressW = WrapingMode::MIRROR;
				else if (value == "CLAMP")
					desc.addressW = WrapingMode::CLAMP;
				else if (value == "BORDER")
					desc.addressW = WrapingMode::BORDER;
				else if (value == "MIRRORONCE")
					desc.addressW = WrapingMode::MIRROR_ONCE;
		}
		else
			QCAT_CORE_ASSERT(false, "AdressWrapping doesnt come Third!");

		std::getline(stream, value, '_');
		if (value == "MipLodBias")
		{
			std::getline(stream, value, '_');
			desc.MipLodBias = std::stof(value);
		}
		else
			QCAT_CORE_ASSERT(false, "MipLodBias doesnt come Forth!");

		std::getline(stream, value, '_');
		if (value == "MaxAnisotropy")
		{
			std::getline(stream, value, '_');
			desc.maxAnisotropy = std::stoi(value);
		}
		else
			QCAT_CORE_ASSERT(false, "MaxAnisotropy doesnt come Fifth!");

		std::getline(stream, value, '_');
		if (value == "ComparisonFunc")
		{
			std::getline(stream, value, '_');
			if (value == "NEVER")
				desc.comparisonFunc = COMPARISON_FUNC::NEVER;
			else if(value =="EQUAL")
				desc.comparisonFunc = COMPARISON_FUNC::EQUAL;
			else if (value == "LESS")
				desc.comparisonFunc = COMPARISON_FUNC::LESS;
			else if (value == "LESSEQUAL")
				desc.comparisonFunc = COMPARISON_FUNC::LESS_EQUAL;
			else if (value == "GREATER")
				desc.comparisonFunc = COMPARISON_FUNC::GREATER;
			else if (value == "GREATEREQUAL")
				desc.comparisonFunc = COMPARISON_FUNC::GREATER_EQUAL;
			else if (value == "NOTEQUAL")
				desc.comparisonFunc = COMPARISON_FUNC::NOT_EQUAL;
			else if (value == "ALWAYS")
				desc.comparisonFunc = COMPARISON_FUNC::ALWAYS;
		}
		else
			QCAT_CORE_ASSERT(false, "ComparisonFunc doesnt come Sixth!");

		std::getline(stream, value, '_');
		if (value == "BorderColor")
		{
			std::getline(stream, value, '_');
			std::stringstream tempstream(value);
			std::getline(tempstream, value, ',');
			float temp = std::stof(value);
			desc.borderColor[0] = temp;
			std::getline(tempstream, value, ',');
			temp = std::stof(value);
			desc.borderColor[1] = temp;
			std::getline(tempstream, value, ',');
			temp = std::stof(value);
			desc.borderColor[2] = temp;
			std::getline(tempstream, value, ',');
			temp = std::stof(value);
			desc.borderColor[3] = temp;
		}
		else
			QCAT_CORE_ASSERT(false, "BorderColor doesnt come Seventh!");

		std::getline(stream, value, '_');
		if (value == "MinMaxLod")
		{
			std::getline(stream, value, '_');
			std::stringstream tempstream(value);
			std::getline(tempstream, value, ',');
			float temp = std::stof(value);
			desc.minLod = temp;
			std::getline(tempstream, value, ',');
			temp = std::stof(value);
			desc.maxLod = temp;
		}
		else
			QCAT_CORE_ASSERT(false, "BorderColor doesnt come Eighth!");

		return desc;
	}
	void SamplerState::SetSamplerDesc(Sampler_Desc desc)
	{
		this->desc = desc;
		Invalidate();
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

