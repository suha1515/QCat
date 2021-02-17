#pragma once

#include "QCat.h"

namespace QCat
{
	class Material
	{
	public:
		enum class MaterialType
		{
			None=0,Diffuse,Specular,NormalMap,Emission
		};
		Material()
			:Material({0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},32.f)
		{}
		Material(glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular,float shininess)
			:ambient(ambient),diffuse(diffuse), specular(specular),shininess(shininess)
		{
			Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
			byte r = 125;
			byte g = 125;
			byte b = 125;
			byte a = 125;

			//unsigned int whiteTextureData = 0xffffffff;
			unsigned int whiteTextureData = r << 24 | g << 16 | b << 8 | a;

			whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
			SetTexture(whiteTexture, Material::MaterialType::Diffuse);
			SetTexture(whiteTexture, Material::MaterialType::Specular);
			SetTexture(whiteTexture, Material::MaterialType::Emission);
			SetTexture(whiteTexture, Material::MaterialType::NormalMap);
		}
		void SetTexture(const std::string& path,MaterialType type);
		void SetTexture(const Ref<Texture2D>& texture, MaterialType type);

		void SetDiffuse(const glm::vec3& color) { diffuse = color; }
		void SetAmbient(const glm::vec3& color) { ambient = color; }
		void SetSpecular(const glm::vec3& color) { specular = color; }
		void SetShininess(const float shininess) { this->shininess = shininess; }

		Ref<Texture2D> GetTexture(MaterialType type);

		bool IsThereTexture(MaterialType type);
		void Bind();
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Ref<Texture2D> m_DiffuseTexture;
		Ref<Texture2D> m_SpecularTexture;
		Ref<Texture2D> m_NormalMapTexture;
		Ref<Texture2D> m_EmissionTexture;

	};
		
}