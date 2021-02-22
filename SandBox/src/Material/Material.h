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
			m_WhiteTexture  = Texture2D::Create(1, 1);
			byte r = 125;
			byte g = 125;
			byte b = 125;
			byte a = 125;

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
	public:
		//Material& operator== (const Material& mat)
		//{

		//}
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Ref<Texture> m_DiffuseTexture;
		Ref<Texture> m_SpecularTexture;
		Ref<Texture> m_NormalMapTexture;
		Ref<Texture> m_EmissionTexture;
		Ref<Texture> m_WhiteTexture;

	};
		
}