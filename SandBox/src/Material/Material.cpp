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
		case MaterialType::Emission:
			m_EmissionTexture = Texture2D::Create(path);
			break;
		}
	}

	void Material::SetTexture(const Ref<Texture2D>& texture, MaterialType type)
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
		case MaterialType::Emission:
			m_EmissionTexture = texture;
			break;
		}
	}

	void Material::Bind()
	{
		if (m_DiffuseTexture)
			m_DiffuseTexture->Bind(0);
		if (m_SpecularTexture)
			m_SpecularTexture->Bind(1);
		if (m_EmissionTexture)
			m_EmissionTexture->Bind(2);
	}
}

