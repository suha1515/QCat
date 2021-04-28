#include "qcpch.h"
#include "Material.h"

namespace QCat
{
	void Material::SetTexture(const std::string& texturepath, Sampler_Desc& desc, TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Diffuse:
			m_DiffuseTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::Specular:
			m_SpecularTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::Normal:
			m_NormalMapTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::Metallic:
			m_MetallicTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::Roughness:
			m_RoughnessTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::AmbientOcclusion:
			m_AmbientOcclusionTexture = TextureLibrary::Load(texturepath, desc);
			break;
		case TextureType::Emission:
			m_EmissionTexture = TextureLibrary::Load(texturepath, desc);
			break;
		}
	}

	void Material::SetTexture(const Ref<Texture>& texture, TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Diffuse:
			m_DiffuseTexture = texture;
			break;
		case TextureType::Specular:
			m_SpecularTexture = texture;
			break;
		case TextureType::Normal:
			m_NormalMapTexture = texture;
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
		case TextureType::Emission:
			m_EmissionTexture = texture;
			break;
		}
	}

	Ref<Texture> Material::GetTexture(TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Diffuse:
			return m_DiffuseTexture;
			break;
		case TextureType::Specular:
			return m_SpecularTexture;
			break;
		case TextureType::Normal:
			return m_NormalMapTexture;
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
		case TextureType::Emission:
			return m_EmissionTexture;
			break;
		}
		return nullptr;
	}

	bool Material::IsThereTexture(TextureType type)
	{
		bool value = false;
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Diffuse:
			if (m_DiffuseTexture)
				value = true;
			break;
		case TextureType::Specular:
			if (m_SpecularTexture)
				value = true;
			break;
		case TextureType::Normal:
			if (m_NormalMapTexture)
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
		case TextureType::Emission:
			if (m_EmissionTexture)
				value = true;
			break;
		}
		return value;
	}

	void Material::Bind()
	{
		if (m_DiffuseTexture)
			m_DiffuseTexture->Bind(0);
		else
			m_WhiteTexture->Bind(0);
		if (m_SpecularTexture)
			m_SpecularTexture->Bind(1);
		else
			m_WhiteTexture->Bind(1);
		if (m_NormalMapTexture)
			m_NormalMapTexture->Bind(2);
		else
			m_WhiteTexture->Bind(2);
		if (m_MetallicTexture)
			m_MetallicTexture->Bind(3);
		else
			m_WhiteTexture->Bind(3);
		if (m_RoughnessTexture)
			m_RoughnessTexture->Bind(4);
		else
			m_WhiteTexture->Bind(4);
		if (m_AmbientOcclusionTexture)
			m_AmbientOcclusionTexture->Bind(5);
		else
			m_WhiteTexture->Bind(5);
		if (m_EmissionTexture)
			m_EmissionTexture->Bind(6);
		else
			m_WhiteTexture->Bind(6);
	}
	void Material::Bind(unsigned int slot, TextureType type)
	{
		switch (type)
		{
		case TextureType::None: QCAT_CORE_ASSERT(false);
			break;
		case TextureType::Diffuse:
			if (m_DiffuseTexture)	m_DiffuseTexture->Bind(slot); else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::Specular:
			if (m_SpecularTexture)
				m_SpecularTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::Normal:
			if (m_NormalMapTexture)
				m_NormalMapTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::Metallic:
			if (m_MetallicTexture)
				m_MetallicTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::Roughness:
			if (m_RoughnessTexture)
				m_RoughnessTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::AmbientOcclusion:
			if (m_AmbientOcclusionTexture)
				m_AmbientOcclusionTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		case TextureType::Emission:
			if (m_EmissionTexture)
				m_EmissionTexture->Bind(slot);
			else
				m_WhiteTexture->Bind(slot);
			break;
		}
	}
}

