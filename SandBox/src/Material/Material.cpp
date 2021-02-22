#include "Material.h"

namespace QCat
{
	void Material::SetTexture(const std::string& path, MaterialType type)
	{
		switch (type)
		{
		case MaterialType::None: QCAT_CORE_ASSERT(false);
			break;
		case MaterialType::Diffuse:
			m_DiffuseTexture = Texture2D::Create(path);
			break;
		case MaterialType::Specular:
			m_SpecularTexture = Texture2D::Create(path);
			break;
		case MaterialType::NormalMap:
			m_NormalMapTexture = Texture2D::Create(path);
			break;
		case MaterialType::Emission:
			m_EmissionTexture = Texture2D::Create(path);
			break;
		}
	}

	void Material::SetTexture(const Ref<Texture>& texture, MaterialType type)
	{
		switch (type)
		{
		case MaterialType::None: QCAT_CORE_ASSERT(false);
			break;
		case MaterialType::Diffuse:
			m_DiffuseTexture = texture;
			break;
		case MaterialType::Specular:
			m_SpecularTexture = texture;
			break;
		case MaterialType::NormalMap:
			m_NormalMapTexture = texture;
			break;
		case MaterialType::Emission:
			m_EmissionTexture = texture;
			break;
		}
	}

	Ref<Texture> Material::GetTexture(MaterialType type)
	{
		switch (type)
		{
		case MaterialType::None: QCAT_CORE_ASSERT(false);
			break;
		case MaterialType::Diffuse:
			return m_DiffuseTexture;
			break;
		case MaterialType::Specular:
			return m_SpecularTexture;
			break;
		case MaterialType::NormalMap:
			return m_NormalMapTexture;
			break;
		case MaterialType::Emission:
			return m_EmissionTexture;
			break;
		}
		return nullptr;
	}

	bool Material::IsThereTexture(MaterialType type)
	{
		switch (type)
		{
		case MaterialType::None: QCAT_CORE_ASSERT(false);
			break;
		case MaterialType::Diffuse:
			if (m_DiffuseTexture)
				return true;
			else
				return false;
			break;
		case MaterialType::Specular:
			if (m_SpecularTexture)
				return true;
			else
				return false;
			break;
		case MaterialType::NormalMap:
			if (m_NormalMapTexture)
				return true;
			else
				return false;
			break;
		case MaterialType::Emission:
			if (m_EmissionTexture)
				return true;
			else
				return false;
			break;
		}
		return false;
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
		if (m_EmissionTexture)
			m_EmissionTexture->Bind(3);
		else
			m_WhiteTexture->Bind(3);
	}
}

