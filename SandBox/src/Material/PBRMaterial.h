//#pragma once
//#include "QCat.h"
//
//namespace QCat
//{
//	class PBRMaterial
//	{
//	public:
//		enum class TextureType
//		{
//			None = 0, Albedo, Normal, 
//		};
//		PBRMaterial()
//			:PBRMaterial({ 0.2f,0.2f,0.2f }, { 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f }, 32.f)
//		{}
//		PBRMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
//			:ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
//		{
//			Sampler_Desc desc;
//			
//			byte r = 255;
//			byte g = 255;
//			byte b = 255;
//			byte a = 255;
//
//			//unsigned int whiteTextureData = 0xffffffff;
//			
//		}
//
//		void SetDiffuse(const glm::vec3& color) { diffuse = color; }
//		void SetAmbient(const glm::vec3& color) { ambient = color; }
//		void SetSpecular(const glm::vec3& color) { specular = color; }
//		void SetShininess(const float shininess) { this->shininess = shininess; }
//
//		void SetTexture(const std::string& texturepath,Sampler_Desc& desc, TextureType type);
//		void SetTexture(Ref<Texture>& texture, TextureType type);
//		Ref<Texture> GetTexture(TextureType type);
//		bool IsThereTexture(TextureType type);
//		void Bind();
//	public:
//		//Material& operator== (const Material& mat)
//		//{
//
//		//}
//	public:
//		glm::vec3 ambient;
//		glm::vec3 diffuse;
//		glm::vec3 specular;
//		float shininess = 32.0f;
//		float metallic;
//		float roughness;
//		float ao;
//
//		Ref<Texture> m_AlbedoTexture;
//		Ref<Texture> m_NormalTexture;
//		
//		Ref<Texture> m_WhiteTexture;
//
//	};
//
//}