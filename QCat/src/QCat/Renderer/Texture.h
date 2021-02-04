#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual void*       GetTexture() const = 0;

		virtual void SetData(void* data, unsigned int size) = 0;

		virtual void Bind(unsigned int slot =0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
		virtual std::string GetPath() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(unsigned int width, unsigned int height);
		static Ref<Texture2D> Create(const std::string & path);
		virtual std::string GetPath() const = 0;
	};

	class TextureLibrary
	{
	public:
		static Ref<Texture2D> Load(const std::string& path)
		{
			return Get().Load_(path);
		}
	private:
		static TextureLibrary& Get()
		{
			static TextureLibrary lib;
			return lib;
		}
		bool Exists(const std::string& path) const
		{
			return m_Textures.find(path) != m_Textures.end();
		}
		Ref<Texture2D> Load_(const std::string& path)
		{
			if (Exists(path))
			{
				return m_Textures[path];
			}
			else
			{
				Ref<Texture2D> texture = Texture2D::Create(path);
				m_Textures[path] = texture;
				return texture;
			}
		}
	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
	};


}
