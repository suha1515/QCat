#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/SamplerState.h"
#include "QCat/Renderer/enums.h"

namespace QCat
{
	struct Texture_Desc
	{
		uint32_t Width;
		uint32_t Height;
		uint32_t Depth=0;
		uint32_t MipLevels =0;
		uint32_t ArraySize =1;
		uint32_t SampleCount=1;
		uint32_t SampleQuality=0;
		TextureFormat Format;
		TextureType Type;
		TextureUsage usage;
		bool gammaCorrection;
	};
	struct QCAT_BOX
	{
		uint32_t xoffset = 0;
		uint32_t yoffset = 0;
		uint32_t x = 0, y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};
	
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual void*       GetTexture() const = 0;
		virtual const Texture_Desc GetDesc() { return desc; };
		virtual void GenerateMipMap()=0;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) = 0;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex=0) = 0;
		virtual void SetSize(uint32_t width, uint32_t height,uint32_t depth=0)=0;

		virtual void Bind(unsigned int slot =0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual TextureType GetTextureType() const { return desc.Type; } ;
		virtual TextureFormat GetTextureFormat() const { return desc.Format; };

		std::string GetTexturePath() { return path; }
		std::string GetSamplerSignature() { return sampler->GetSignature(); }
		//virtual Texture& operator=(const Texture& other) = 0;

		Ref<SamplerState> sampler;
		Texture_Desc desc;
		std::string path = "none";
	};

	class Texture2D : public Texture
	{
	public:
		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) = 0;

		static Ref<Texture2D> Create(TextureFormat format,Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1,void* pData = nullptr);
		static Ref<Texture2D> Create(const std::string & path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1,bool flip =false,bool gamacorrection = false);

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
		static Ref<Texture2D>& Load(const std::string& path, Sampler_Desc desc = {}, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false, bool gamaCorrection = false)
		{
			return Get().Load_(path, desc,mipLevel,samples,flip,gamaCorrection);
		}
		static Ref<Texture2D>& Load(const std::string& name, TextureFormat format, unsigned int width, unsigned int height,void* pData, Sampler_Desc desc = {},unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false)
		{
			return Get().Load_(name, format,width,height,pData,desc, mipLevel, samples, flip);
		}
		static std::vector<std::string>& GetTexturePathList()
		{
			return Get().texturepathlist;
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
		Ref<Texture2D>& Load_(const std::string& path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip=false,  bool gamaCorrection = false)
		{
			if (Exists(path))
			{
				return m_Textures[path];
			}
			else
			{
				texturepathlist.push_back(path);
				Ref<Texture2D> texture = Texture2D::Create(path, desc,mipLevel,samples,flip, gamaCorrection);
				m_Textures[path] = texture;
				return m_Textures[path];
			}
		}
		Ref<Texture2D>& Load_(const std::string& name, TextureFormat format, unsigned int width, unsigned int height, void* pData, Sampler_Desc desc,unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false)
		{
			if (Exists(name))
			{
				return m_Textures[name];
			}
			else
			{
				Ref<Texture2D> texture = Texture2D::Create(format,desc,width,height, mipLevel, samples,pData);
				m_Textures[name] = texture;
				return m_Textures[name];
			}
		}
	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
		std::vector <std::string> texturepathlist;
	};
	class TextureView
	{
	public:
		TextureView() = default;
		virtual ~TextureView() = default;
	private:

	};
	class TextureUtility
	{
	public:
		TextureUtility() = default;
		virtual ~TextureUtility() = default;

		static void CopyTexture2D(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion)
		{
			Get_()->CopyTexture2D_(srcTex, dstTex, mipLevel, boxregion);
		}
		static void CopyCubeMapFace2D(Ref<Texture>& srcCubeMap, Ref<Texture>& dstTex, uint32_t index, uint32_t mipLevel, QCAT_BOX boxregion)
		{
			Get_()->CopyCubemapFace2D_(srcCubeMap, dstTex, index, mipLevel, boxregion);
		}
	protected:
		virtual void CopyTexture2D_(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion) = 0;
		virtual void CopyCubemapFace2D_(Ref<Texture>& srcCubeMap, Ref<Texture>& dstTex, uint32_t index, uint32_t mipLevel, QCAT_BOX boxregion) = 0;
	private:
		static Ref<TextureUtility> Get_();

	};

}
