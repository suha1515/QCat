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
			:Material({0.2f,0.2f,0.2f},{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},32.f)
		{}
		Material(glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular,float shininess)
			:ambient(ambient),diffuse(diffuse), specular(specular),shininess(shininess)
		{
			Sampler_Desc desc;
			m_WhiteTexture  = Texture2D::Create(desc ,1, 1);
			byte r = 255;
			byte g = 255;
			byte b = 255;
			byte a = 255;

			//unsigned int whiteTextureData = 0xffffffff;
			unsigned int whiteTextureData = r << 24 | g << 16 | b << 8 | a;

			m_WhiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
		}
		void SetTexture(const std::string& path,MaterialType type);
		void SetTexture(const Ref<Texture>& texture, MaterialType type);

		void SetDiffuse(const glm::vec3& color) { diffuse = color; }
		void SetAmbient(const glm::vec3& color) { ambient = color; }
		void SetSpecular(const glm::vec3& color) { specular = color; }
		void SetShininess(const float shininess) { this->shininess = shininess; }

		Ref<Texture> GetTexture(MaterialType type);

		bool IsThereTexture(MaterialType type);
		void Bind();
		void Bind(int slot,MaterialType type);
	public:
		//Material& operator== (const Material& mat)
		//{

		//}
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess =32.0f;
		float metallic;
		float roughness;
		float ao;

		Ref<Texture> m_DiffuseTexture;
		Ref<Texture> m_SpecularTexture;
		Ref<Texture> m_NormalMapTexture;
		Ref<Texture> m_EmissionTexture;
		Ref<Texture> m_WhiteTexture;

	};
		
}