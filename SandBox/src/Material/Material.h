#pragma once

#include "QCat.h"

namespace QCat
{
	class Material
	{
	public:
		enum class MaterialType
		{
			None=0,Diffuse,Specular,Emission
		};
		Material(glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular,float shininess)
			:ambient(ambient),diffuse(diffuse), specular(specular),shininess(shininess)
		{}
		void SetTexture(const std::string& path,MaterialType type);
		void SetTexture(const Ref<Texture2D>& texture, MaterialType type);
		void Bind();
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Ref<Texture2D> m_DiffuseTexture;
		Ref<Texture2D> m_SpecularTexture;
		Ref<Texture2D> m_EmissionTexture;
	};
		
}