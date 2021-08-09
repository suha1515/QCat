#pragma once
#include "QCat/Core/QCatCore.h"
#include "Texture.h"

namespace QCat
{
	class Material
	{
	public:
		enum class TextureType
		{
			None = 0, Diffuse, Specular, Normal, Metallic, Roughness, AmbientOcclusion, Emission
		};
		Material()
			:Material({ 0.2f,0.2f,0.2f }, { 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f }, 32.f)
		{}
		Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
			:ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
		{
			Sampler_Desc desc;
			unsigned char r = 255;
			unsigned char g = 255;
			unsigned char b = 255;
			unsigned char a = 255;

			//unsigned int whiteTextureData = 0xffffffff;
			unsigned int whiteTextureData = r << 24 | g << 16 | b << 8 | a;
			m_WhiteTexture = TextureLibrary::Load("white1x1tex", TextureFormat::RGBA8, 1, 1, &whiteTextureData);
		}
		void SetTexture(const std::string& texturepath, Sampler_Desc& desc, TextureType type);
		void SetTexture(const Ref<Texture>& texture, TextureType type);

		void SetDiffuse(const glm::vec3& color) { diffuse = color; }
		void SetAmbient(const glm::vec3& color) { ambient = color; }
		void SetSpecular(const glm::vec3& color) { specular = color; }
		void SetShininess(const float shininess) { this->shininess = shininess; }

		Ref<Texture> GetTexture(TextureType type);

		bool IsThereTexture(TextureType type);
		void Bind();
		void Bind(unsigned int slot, TextureType type);
	public:
		//Material& operator== (const Material& mat)
		//{

		//}
		std::string name = "";
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess = 32.0f;
		float metallic = 0.1f;
		float roughness = 1.0f;
		float ao = 1.0f;

		Ref<Texture> m_DiffuseTexture;
		Ref<Texture> m_SpecularTexture;
		Ref<Texture> m_NormalMapTexture;
		Ref<Texture> m_EmissionTexture;
		// PBR texture
		Ref<Texture> m_MetallicTexture;
		Ref<Texture> m_RoughnessTexture;
		Ref<Texture> m_AmbientOcclusionTexture;
		// Default
		Ref<Texture> m_WhiteTexture;

	};
	class MaterialLibrary
	{
	public:
		static Ref<Material> GetMaterial(const std::string& name)
		{
			return Get().GetMaterial_(name);
		}
		static Ref<Material> SetMaterial(Material& material)
		{
			Ref<Material> mat = CreateRef<Material>();
			*mat = material;
			return Get().SetMaterial_(mat->name, mat);
		}
	private:
		bool IsExist(const std::string& name)
		{
			const auto& iter = m_materials.find(name);
			if (iter != m_materials.end())
				return true;
			else
				return false;
		}
		Ref<Material> GetMaterial_(const std::string& name)
		{
			if(IsExist(name))
				return m_materials[name];
			else
			{
				QCAT_CORE_ERROR("There is no Material ID : {0}", name);
				return nullptr;
			}
		}
		Ref<Material> SetMaterial_(const std::string& name, Ref<Material>& material)
		{
			if (!IsExist(name))
				return m_materials[name] = material;
			else
			{
				QCAT_CORE_ERROR("There is already Material at ID : {0}", name);
				return nullptr;
			}
		}
		static MaterialLibrary& Get()
		{
			static MaterialLibrary lib;
			return lib;
		}
		std::unordered_map<std::string,Ref<Material>> m_materials;
	};
}