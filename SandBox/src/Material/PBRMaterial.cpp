#include "PBRMaterial.h"

namespace QCat
{
	void PBRMaterial::SetTexture(const std::string& texturepath, Sampler_Desc& desc, TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Albedo:
			m_AlbedoTexture = Texture2D::Create(texturepath, desc);
			break;
		case TextureType::Normal:
			m_NormalTexture = Texture2D::Create(texturepath, desc);
			break;
		case TextureType::Metallic:
			m_MetallicTexture = Texture2D::Create(texturepath, desc);
			break;
		case TextureType::Roughness:
			m_RoughnessTexture = Texture2D::Create(texturepath, desc);
			break;
		case TextureType::AmbientOcclusion:
			m_AmbientOcclusionTexture = Texture2D::Create(texturepath, desc);
			break;
		}
	}
	void PBRMaterial::SetTexture(Ref<Texture>& texture, TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Albedo:
			m_AlbedoTexture = texture;
			break;
		case TextureType::Normal:
			m_NormalTexture = texture;
			break;
		case TextureType::Metallic:
			m_MetallicTexture = texture;
			break;
		case TextureType::Roughness:
			m_RoughnessTexture = texture;
			break;
		case TextureType::AmbientOcclusion:
			m_AmbientOcclusionTexture = texture;
			break;
		}
	}
	Ref<Texture> PBRMaterial::GetTexture(TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Albedo:
			return m_AlbedoTexture;
			break;
		case TextureType::Normal:
			return m_NormalTexture;
			break;
		case TextureType::Metallic:
			return m_MetallicTexture;
			break;
		case TextureType::Roughness:
			return m_RoughnessTexture;
			break;
		case TextureType::AmbientOcclusion:
			return m_AmbientOcclusionTexture;
			break;
		}
	}
	bool PBRMaterial::IsThereTexture(TextureType type)
	{
		bool value = false;
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Albedo:
			if (m_AlbedoTexture)
				value =  true;
			break;
		case TextureType::Normal:
			if (m_NormalTexture)
				value = true;
			break;
		case TextureType::Metallic:
			if (m_MetallicTexture)
				value = true;
			break;
		case TextureType::Roughness:
			if (m_RoughnessTexture)
				value = true;
			break;
		case TextureType::AmbientOcclusion:
			if (m_AmbientOcclusionTexture)
				value = true;
			break;
		}
		return value;
	}
	void PBRMaterial::Bind()
	{
		if (m_AlbedoTexture)
			m_AlbedoTexture->Bind(0);
		else
			m_WhiteTexture->Bind(0);
		if (m_NormalTexture)
			m_NormalTexture->Bind(1);
		else
			m_WhiteTexture->Bind(1);
		if (m_MetallicTexture)
			m_MetallicTexture->Bind(2);
		else
			m_WhiteTexture->Bind(2);
		if (m_RoughnessTexture)
			m_RoughnessTexture->Bind(3);
		else
			m_WhiteTexture->Bind(3);
		if (m_AmbientOcclusionTexture)
			m_AmbientOcclusionTexture->Bind(4);
		else
			m_WhiteTexture->Bind(4);

	}
}