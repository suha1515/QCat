#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/SamplerState.h"
#include "QCat/Renderer/enums.h"
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
		
		//virtual Texture& operator=(const Texture& other) = 0;

		Ref<SamplerState> sampler;
	};

	class Texture2D : public Texture
	{
	public:
		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) = 0;

		static Ref<Texture2D> Create(TextureFormat format,Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1,void* pData = nullptr);
		static Ref<Texture2D> Create(Sampler_Desc desc,unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1);
		static Ref<Texture2D> Create(const std::string & path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1,bool flip =false,bool gamacorrection = false);
	private:
		std::string path;
	};
	class TextureCube  : public Texture
	{
	public:
		virtual void ReadData(uint32_t face,uint32_t x, uint32_t y, const void* outdata) = 0;

		static Ref<TextureCube> Create(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel=1, bool flip=false,bool gammacorrection = false);
		static Ref<TextureCube> Create(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, void* pData = nullptr);


	};

	class TextureLibrary
	{
	public:
		static Ref<Texture2D> Load(const std::string& path, Sampler_Desc desc = {}, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false, bool gamaCorrection = false)
		{
			return Get().Load_(path, desc,mipLevel,samples,flip,gamaCorrection);
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
		Ref<Texture2D> Load_(const std::string& path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip=false,  bool gamaCorrection = false)
		{
			if (Exists(path))
			{
				return m_Textures[path];
			}
			else
			{
				Ref<Texture2D> texture = Texture2D::Create(path, desc,mipLevel,samples,flip, gamaCorrection);
				m_Textures[path] = texture;
				return texture;
			}
		}
	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
	};


}
