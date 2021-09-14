#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
#include "DXUtils.h"
#include "DX11_Sampler.h"
#include "QCat/Uitiliy/Float16.h"
#include "glm/gtc/type_ptr.hpp"
namespace QCat
{
	namespace Utils
	{
		void* LoadImageFromFile(const std::string& path, bool gamacorrection, bool flip, uint32_t& width, uint32_t& height, uint32_t channel, DXGI_FORMAT& format)
		{
			int width_, height_, channels_;
			if (!flip)
				stbi_set_flip_vertically_on_load(0);
			else
				stbi_set_flip_vertically_on_load(1);
			auto begin = path.find_last_of('.');
			std::string extension = path.substr(begin + 1, path.length());
			void* stb_data = nullptr;
			void* pData = nullptr;
			Float16* float16Array = nullptr;
			unsigned char* UData = nullptr;

			if (extension != "hdr")
			{
				stb_data = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load Image!");

			}
			else
			{
				stb_data = stbi_loadf(path.c_str(), &width_, &height_, &channels_, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load HDR image!");

				float16Array = new Float16[width * height * 4];
				for (unsigned int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
				{
					float16Array[i] = ((float*)stb_data)[j];
					float16Array[i + 1] = ((float*)stb_data)[j + 1];
					float16Array[i + 2] = ((float*)stb_data)[j + 2];
					float16Array[i + 3] = 1.0f;
				}
			}

			width = width_;
			height = height_;
			channel = channels_;
			{
				if (extension != "hdr")
				{
					switch (channel)
					{
					case 4:
						format = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0; i < width * height * 4; ++i)
							UData[i] = ((unsigned char*)stb_data)[i];
						break;
					case 3:
						format = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
						{
							UData[i] = ((unsigned char*)stb_data)[j];
							UData[i + 1] = ((unsigned char*)stb_data)[j + 1];
							UData[i + 2] = ((unsigned char*)stb_data)[j + 2];
							UData[i + 3] = 255;
						}
						break;
					case 2:
						format = gamacorrection ? DXGI_FORMAT_R8G8_SNORM : DXGI_FORMAT_R8G8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0, j = 0; j < width * height * 2; i += 4, j += 2)
						{
							UData[i] = ((unsigned char*)stb_data)[j];
							UData[i + 1] = ((unsigned char*)stb_data)[j + 1];
							UData[i + 2] = 255;
							UData[i + 3] = 255;
						}
						break;
					case 1:
						UData = new unsigned char[width * height];
						format = DXGI_FORMAT_R8_UNORM;
						for (int i = 0; i < width * height; ++i)
							UData[i] = ((unsigned char*)stb_data)[i];
						break;
					}
					pData = UData;
				}
				else
				{
					switch (channel)
					{
					case 4:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0; i < width * height * 4; i++)
							float16Array[i] = ((float*)stb_data)[i];
						break;
					case 3:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = ((float*)stb_data)[j + 1];
							float16Array[i + 2] = ((float*)stb_data)[j + 2];
							float16Array[i + 3] = 1.0f;
						}
						break;
					case 2:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height * 2; i += 4, j += 2)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = ((float*)stb_data)[j + 1];
							float16Array[i + 2] = 1.0f;
							float16Array[i + 3] = 1.0f;
						}
						break;
					case 1:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height; i += 4, j += 1)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = 1.0f;
							float16Array[i + 2] = 1.0f;
							float16Array[i + 3] = 1.0f;
						}
						break;


					}
					pData = float16Array;
					format = DXGI_FORMAT_R16G16B16A16_FLOAT;
					//m_dataFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}
			return pData;
		}
	}
	DX11Texture2D::DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gammacorrection )
	{
		QCAT_PROFILE_FUNCTION();
		this->pathes.push_back(path);
		
		// Texture Description
		uint32_t width = 0, height = 0, channels = 0;
		DXGI_FORMAT format;
		void* pData = nullptr;
		pData = Utils::LoadImageFromFile(path, gammacorrection, flip, width, height, channels, format);
		m_dataFormat = format;
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET ;
		textureDesc.CPUAccessFlags = 0;
		if (mipLevel !=1 && mipLevel>=0)
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(format);
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Invalidate(pData);
		sampler = DX11Sampler::Create(desc);

		if (pData != nullptr)
		{
			free(pData);
			pData = nullptr;
		}
	}
	DX11Texture2D::DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, TextureUsage usage , void* pData)
	{
		m_dataFormat = Utils::GetDirectDataType(format);

		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = Utils::GetUsageDX(usage);
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS && m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET| D3D11_BIND_UNORDERED_ACCESS;
		else
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = usage;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Invalidate(pData);

		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	void DX11Texture2D::GenerateMipMap()
	{
		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap");
	}
	void DX11Texture2D::SetData(void* data, unsigned int size, uint32_t textureindex )
	{
		QCAT_PROFILE_FUNCTION();

		//unsigned int bpc = m_dataFormat == DXGI_FORMAT_R8G8B8A8_UNORM ? 4 : 3;
		//QCAT_CORE_ASSERT(size == m_width * m_height * bpc, "Data must be entire texture!");z
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, size, 0u);
	}
	void DX11Texture2D::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		textureDesc.Width = width;
		textureDesc.Height = height;
		this->desc.Width = width;
		this->desc.Height = height;

		Invalidate(nullptr);
	}
	void DX11Texture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		sampler->Bind(slot);
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	void DX11Texture2D::ReadData(uint32_t miplevel, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, uint32_t bufSize, void* pixels)
	{
			namespace wrl = Microsoft::WRL;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
		// Get Texture Resource from View
		/*wrl::ComPtr<ID3D11Resource> pResSource;
		pTextureView->GetResource(&pResSource);
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		*/
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = Utils::GetDirectDataType(format);
		textureDesc.SampleDesc.Count = this->desc.SampleCount;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;

		//텍스처 생성
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

		// 텍스처 복사
		D3D11_BOX srcBox;
		srcBox.left = xoffset;
		srcBox.right = xoffset + width;
		srcBox.bottom = yoffset + height;
		srcBox.top = yoffset;
		srcBox.front = 0;
		srcBox.back = 1;
		gfx.GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexture.Get(), 0, &srcBox);

		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx.GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		if (xoffset >= 0 && xoffset <= this->desc.Width && yoffset >= 0 && yoffset <= this->desc.Height)
		{
			if(dataType == TextureDataType::INT)
				*reinterpret_cast<int*>(pixels) = *reinterpret_cast<int*>(msr.pData);
		}
		gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	}
	template<typename T>
	void MapProcessing(uint32_t miplevel, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height,void* dstData,const void* srcdata)
	{
		for (int x = xoffset; x < width; ++x)
		{
			for (int y = xoffset; y < height; ++y)
			{
				T* ptr = reinterpret_cast<T*>(dstData);
				ptr += width * height * sizeof(T);
				*ptr = *reinterpret_cast<T*>(const_cast<void*>(srcdata));
			}
		}
	}
	void DX11Texture2D::ClearData(uint32_t miplevel, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, const void* data)
	{
		namespace wrl = Microsoft::WRL;
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		uint32_t sizeData = Utils::GetDataTypeSize(dataType);
		D3D11_MAPPED_SUBRESOURCE msr = {};

		switch(textureDesc.Usage)
		{
			case D3D11_USAGE_DYNAMIC:
			{
				gfx.GetContext()->Map(pTexture.Get(), miplevel, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &msr);
				if (xoffset >= 0 && xoffset <= this->desc.Width && yoffset >= 0 && yoffset <= this->desc.Height)
				{
					if (dataType == TextureDataType::INT)
						MapProcessing<int>(miplevel, xoffset, yoffset, width, height, msr.pData, data);
				}
				gfx.GetContext()->Unmap(pTexture.Get(), 0);
			}
				break;
			case D3D11_USAGE_STAGING:
			case D3D11_USAGE_DEFAULT:
			{
				if (dataType == TextureDataType::INT)
				{
					int* pdata = new int[width * height];
					int value = *reinterpret_cast<int*>(const_cast<void*>(data));
					for (int i = 0; i < width * height; ++i)
					{
						pdata[i] = value;
					}
					D3D11_BOX srcBox;
					srcBox.left = xoffset;
					srcBox.right = xoffset + width;
					srcBox.bottom = yoffset + height;
					srcBox.top = yoffset;
					srcBox.front = 0;
					srcBox.back = 1;
					gfx.GetContext()->UpdateSubresource(pTexture.Get(), miplevel, &srcBox, pdata, width * sizeData, 0);
					delete[] pdata;
				}
			}
			break;
			case D3D11_USAGE_IMMUTABLE:
				QCAT_CORE_ERROR("Texture Error : Immutable Texture can be changed! ");
			break;
		}
	}
	void DX11Texture2D::Invalidate(void * pData)
	{
		if (pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();

		if (pData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = pData;
			subResource.SysMemPitch = textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format);
			subResource.SysMemSlicePitch = 0;
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, &subResource, &pTexture);
		}
		else
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(m_dataFormat);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		if (textureDesc.MipLevels > 0 &&(textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
	}
	DX11Texture2DArray::DX11Texture2DArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel, unsigned int samples)
	{
		QCAT_PROFILE_FUNCTION();
		m_dataFormat = Utils::GetDirectDataType(format);

		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = depth;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS && m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Type = TextureType::Texture2DArray;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Validate();

		sampler = DX11Sampler::Create(desc);
	}

	DX11Texture2DArray::DX11Texture2DArray(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gamacorrection)
	{
		QCAT_PROFILE_FUNCTION();

		std::vector<void*> pData;
		pData.resize(imagePath.size());
		uint32_t width = 0, height = 0, channel = 0;
		DXGI_FORMAT format;
		pData[0] = Utils::LoadImageFromFile(imagePath[0].c_str(), gamacorrection, flip, width, height, channel, format);

		for (int i = 1; i < imagePath.size(); ++i)
		{
			uint32_t width_ = 0, height_ = 0, channel_ = 0;
			DXGI_FORMAT format_;
			pData[i] = Utils::LoadImageFromFile(imagePath[0].c_str(), gamacorrection, flip, width_, height_, channel_, format_);
			if (width != width_ || height != height_ || channel != channel_ || format != format_)
			{
				QCAT_CORE_ASSERT(false, "DX11TextureCube image isnt same!");
				break;
			}
		}
		m_dataFormat = format;

		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = imagePath.size();
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS && m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Type = TextureType::Texture2DArray;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Validate(pData);

		// free loaded image
		for (int i = 0; i < imagePath.size(); ++i)
		{
			free(pData[i]);
		}
		sampler = DX11Sampler::Create(desc);
	}

	void DX11Texture2DArray::GenerateMipMap()
	{
	}

	void DX11Texture2DArray::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
	}

	void DX11Texture2DArray::GetData(void* data, uint32_t mipLevel, uint32_t textureindex)
	{
	}

	void DX11Texture2DArray::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
	}

	void DX11Texture2DArray::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		sampler->Bind(slot);
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	void DX11Texture2DArray::Validate(std::vector<void*> pData)
	{
		if (pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();

		if (pData.size() > textureDesc.ArraySize )
		{
			QCAT_CORE_ASSERT(false, "Image count is over textureArraySize!");
		}
		else if (pData.size() == 0)
		{
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		}
		else
		{
			std::vector<D3D11_SUBRESOURCE_DATA> subResource;
			subResource.resize(pData.size());
			for (int i = 0; i < pData.size(); ++i)
			{
				subResource[i].pSysMem = pData[i];
				subResource[i].SysMemPitch = textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format);
				subResource[i].SysMemSlicePitch = 0;
			}
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, subResource.data(), &pTexture);
		}
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(m_dataFormat);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = -1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
	}
	void DX11Texture2DArray::Update(void* pData, uint32_t index,uint32_t mipSlice)
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(),
			D3D11CalcSubresource(mipSlice,index, textureDesc.MipLevels), nullptr, pData,
			textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format), 0);
	}
	DX11TextureCube::DX11TextureCube(const std::vector<std::string>& imgPathes, Sampler_Desc desc, unsigned int mipLevel,bool flip, bool gammaCorrection)
	{
		QCAT_PROFILE_FUNCTION();
		void* pData[6];
		uint32_t width = 0, height = 0, channel = 0;
		DXGI_FORMAT format;
		pData[0] = Utils::LoadImageFromFile(imgPathes[0].c_str(), gammaCorrection, flip, width, height, channel, format);

		for (int i = 1; i < 6; ++i)
		{
			uint32_t width_ = 0, height_ = 0, channel_ = 0;
			DXGI_FORMAT format_;
			pData[i] = Utils::LoadImageFromFile(imgPathes[0].c_str(), gammaCorrection, flip, width_, height_, channel_, format_);
			if (width != width_ || height != height_ || channel != channel_ || format != format_)
			{
				QCAT_CORE_ASSERT(false, "DX11TextureCube image isnt same!");
				break;
			}
		}
		m_dataFormat = format;
		// Texture Description
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE ;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE ;
		textureDesc.CPUAccessFlags = 0;
		if (m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS || m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		else
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = Utils::GetTextureFormatFromDX(format);
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = 1;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;
		// Create Texture and update it
		D3D11_SUBRESOURCE_DATA subResource[6];
		for (int i = 0; i < 6; ++i)
		{
			subResource[i].pSysMem = pData[i];
			subResource[i].SysMemPitch = textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format);
			subResource[i].SysMemSlicePitch = 0;
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc,subResource, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		// free loaded image
		for (int i = 0; i < 6; ++i)
		{
			free(pData[i]);
		}
		sampler = DX11Sampler::Create(desc);
	}
	DX11TextureCube::DX11TextureCube(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, void* pData)
	{
		m_dataFormat = Utils::GetDirectDataType(format);
		// Texture Description
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS || m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		else
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = format;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = 1;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		// Create Texture and update it
		Invalidate(pData);
		sampler = DX11Sampler::Create(desc);
	}
	void DX11TextureCube::GenerateMipMap()
	{
		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap");
	}
	void DX11TextureCube::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
	}
	void DX11TextureCube::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		textureDesc.Width = width;
		textureDesc.Height = height;
		this->desc.Width = width;
		this->desc.Height = height;
		Invalidate(nullptr);
	}
	void DX11TextureCube::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		sampler->Bind(slot);
	}
	void DX11TextureCube::ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata)
	{
		namespace wrl = Microsoft::WRL;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
		wrl::ComPtr<ID3D11Resource> pResSource;
		pTextureView->GetResource(&pResSource);
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC desc;

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = 1;
		textureDesc.Height = 1;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 0;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;

		//텍스처 생성
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

		//// 텍스처 복사
		//gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
		int i = D3D11CalcSubresource(0, face, 1);

		D3D11_BOX srcBox;
		srcBox.left = x;
		srcBox.right = x + 1;
		srcBox.bottom = y + 1;
		srcBox.top = y;
		srcBox.front = 0;
		srcBox.back = 1;
		gfx.GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), i, &srcBox);

		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx.GetContext()->Map(pTexTemp.Get(),0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		if (x >= 0 && x <= this->desc.Width && y >= 0 && y <= this->desc.Height)
		{
			outdata = (msr.pData);
		}
		gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	}
	void DX11TextureCube::Invalidate(void* pData)
	{
		if(pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();


		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		if (pData != nullptr)
		{
			for (int i = 0; i < 6; ++i)
			{
				QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), D3D11CalcSubresource(0, i, textureDesc.MipLevels), nullptr, pData, textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format), 0);
			}
		}
		
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(textureDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
	}
	DX11TextureCubeArray::DX11TextureCubeArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel, unsigned int samples )
	{
		QCAT_PROFILE_FUNCTION();
		m_dataFormat = Utils::GetDirectDataType(format);

		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = depth * 6;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS && m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 1 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Type = TextureType::TextureCubeArray;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = depth * 6;
		this->desc.bindFlags = textureDesc.BindFlags;

		Validate();

		sampler = DX11Sampler::Create(desc);
	}
	void DX11TextureCubeArray::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		sampler->Bind(slot);
	}
	void DX11TextureCubeArray::Validate()
	{
		if (pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(m_dataFormat);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srvDesc.TextureCubeArray.First2DArrayFace = 0;
		srvDesc.TextureCubeArray.MipLevels = desc.MipLevels;
		srvDesc.TextureCubeArray.MostDetailedMip = 0;
		srvDesc.TextureCubeArray.NumCubes = (desc.ArraySize/6);

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
	}
	DX11ShaderView::DX11ShaderView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t viewDimension = Utils::GetDimensionFromType(type);
		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);

		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D_SRV_DIMENSION dstDimension = Utils::GetSRVDimensionFromType(type, multisampled);

		bool pass = true;
		if (viewDimension != srcDimension)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : Texture Dimension isnt Compitable!");
			pass = false;
		}
		if (srcTexDesc.MipLevels < startMip || srcTexDesc.MipLevels < startMip + numMip)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		srvDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		srvDesc.ViewDimension = dstDimension;

		if (numlayer > 1 || startLayer > 0)
		{
			if (TextureType::Texture1D == type )
				dstDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			else if (TextureType::Texture2D == type )
				dstDimension = multisampled ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			else if (TextureType::TextureCube == type)
				dstDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;

			srvDesc.ViewDimension = dstDimension;
		}

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;
		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layercount = numlayer;
		uint32_t layerstart = startLayer;
		uint32_t mipcount = numMip;
		uint32_t mipstart = startMip;
		
		switch (dstDimension)
		{
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			srvDesc.Texture1D.MipLevels = mipcount;
			srvDesc.Texture1D.MostDetailedMip = mipstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			srvDesc.Texture1DArray.MipLevels = mipcount;
			srvDesc.Texture1DArray.MostDetailedMip = mipstart;
			srvDesc.Texture1DArray.ArraySize = layercount;
			srvDesc.Texture1DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			srvDesc.Texture2D.MipLevels = mipcount;
			srvDesc.Texture2D.MostDetailedMip = mipstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			srvDesc.Texture2DArray.MipLevels = mipcount;
			srvDesc.Texture2DArray.MostDetailedMip = mipstart;
			srvDesc.Texture2DArray.ArraySize = layercount;
			srvDesc.Texture2DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			srvDesc.Texture2DMSArray.ArraySize = layercount;
			srvDesc.Texture2DMSArray.FirstArraySlice = layerstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			srvDesc.TextureCube.MipLevels = mipcount;
			srvDesc.TextureCube.MostDetailedMip = mipstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
			srvDesc.TextureCubeArray.MipLevels = mipcount;
			srvDesc.TextureCubeArray.MostDetailedMip = mipstart;
			srvDesc.TextureCubeArray.NumCubes = layercount;
			srvDesc.TextureCubeArray.First2DArrayFace = layerstart;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE3D:
			srvDesc.Texture3D.MipLevels = mipcount;
			srvDesc.Texture3D.MostDetailedMip = mipstart;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for ShaderResoruceView!");
			break;
		}
			ID3D11Resource* textureresource=nullptr;
			if (texture1D != nullptr)
				textureresource = texture1D;
			else if (texture2D != nullptr)
				textureresource = texture2D;
			else if (texture3D != nullptr)
				textureresource = texture3D;
			// Create ShaderResouceView
			HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
				textureresource, &srvDesc, &pShaderView
			);
			if (result != S_OK)
			{
				QCAT_CORE_ERROR("Texture View Contruction Failed!");
			}
			if (srcTexDesc.MipLevels > 0 && (srcTexDesc.bindFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
				QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pShaderView.Get());
	}
	DX11ShaderView::~DX11ShaderView()
	{
	}

	void DX11ShaderView::Bind(uint32_t slot, ShaderType type) const
	{
		QCAT_PROFILE_FUNCTION();
		switch (type)
		{
		case ShaderType::VS:
			QGfxDeviceDX11::GetInstance()->GetContext()->VSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::GS:
			QGfxDeviceDX11::GetInstance()->GetContext()->GSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::PS:	
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::CS:
			QGfxDeviceDX11::GetInstance()->GetContext()->CSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		}
	}

	void DX11ShaderView::UnBind(uint32_t slot, ShaderType type) const
	{
		QCAT_PROFILE_FUNCTION();
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		switch (type)
		{
		case ShaderType::VS:
			QGfxDeviceDX11::GetInstance()->GetContext()->VSSetShaderResources(slot, 1u, nullSRV);
			break;
		case ShaderType::GS:
			QGfxDeviceDX11::GetInstance()->GetContext()->GSSetShaderResources(slot, 1u, nullSRV);
			break;
		case ShaderType::PS:
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, nullSRV);
			break;
		case ShaderType::CS:
			QGfxDeviceDX11::GetInstance()->GetContext()->CSSetShaderResources(slot, 1u, nullSRV);
			break;
		}
	}

	DX11RenderTargetView::DX11RenderTargetView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip,  uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);
		//uint32_t dstDimension = Utils::GetDimensionFromType(type);
		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D11_RTV_DIMENSION dstDimension = Utils::GetRTVDimensionFromType(type, multisampled);

		bool pass = true;
		if (srcTexDesc.MipLevels < startMip )
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		rtvDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		rtvDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;

		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		if (numlayer > 1 || startLayer > 0)
		{
			if (TextureType::Texture1D == type)
				dstDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			else if (TextureType::Texture2D == type)
				dstDimension = multisampled ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.ViewDimension = dstDimension;
		}
		uint32_t layercount = numlayer;
		switch (dstDimension)
		{
		case D3D11_RTV_DIMENSION_TEXTURE1D:
			rtvDesc.Texture1D.MipSlice = startMip;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
			rtvDesc.Texture1DArray.MipSlice = startMip;
			rtvDesc.Texture1DArray.ArraySize = numlayer;
			rtvDesc.Texture1DArray.FirstArraySlice = startLayer;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2D:
			rtvDesc.Texture2D.MipSlice = startMip;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2DMS:		
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
			rtvDesc.Texture2DArray.MipSlice = startMip;
			rtvDesc.Texture2DArray.ArraySize = layercount;
			rtvDesc.Texture2DArray.FirstArraySlice = startLayer;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
			rtvDesc.Texture2DMSArray.ArraySize = layercount;
			rtvDesc.Texture2DMSArray.FirstArraySlice = startLayer;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE3D:
			rtvDesc.Texture3D.MipSlice = startMip;
			rtvDesc.Texture3D.FirstWSlice = startLayer;
			rtvDesc.Texture3D.WSize = numlayer;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for RenderTargetView!");
			break;
		}
		ID3D11Resource* textureresource=nullptr;
		if (texture1D != nullptr)
			textureresource = texture1D;
		else if (texture2D != nullptr)
			textureresource = texture2D;
		else if (texture3D != nullptr)
			textureresource = texture3D;
		// Create ShaderResouceView
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateRenderTargetView(
			textureresource, &rtvDesc, &pRenderTargetView
		);
		if (result != S_OK)
		{
			QCAT_CORE_ERROR("RenderTargetView Contruction Failed!");
		}
	}
	DX11RenderTargetView::~DX11RenderTargetView()
	{
	}

	void DX11RenderTargetView::Clear(const glm::vec4& color)
	{
		auto& gfx = QGfxDeviceDX11::GetInstance()->GetContext();
		gfx->ClearRenderTargetView(pRenderTargetView.Get(), glm::value_ptr(color));
	}

	DX11DepthStencilView::DX11DepthStencilView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip,  uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);

		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D11_DSV_DIMENSION dstDimension = Utils::GetDSVDimenstionFromType(type, multisampled);

		bool pass = true;
		if (srcTexDesc.MipLevels < startMip)
		{
			QCAT_CORE_ERROR("DepthStenilView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("DepthStenilView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		viewDesc = srcTexDesc;

		dsvDesc.Format = Utils::MapTypeDSV(viewTexFormat);
		dsvDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;
		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layerstart = startLayer;
		uint32_t layercount = numlayer;
		uint32_t mipstart = startMip;
		//if (TextureType::TextureCubeArray == type)
		//{
		//	dstDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		//}

		if (numlayer > 1 || startLayer > 0)
		{
			if (TextureType::Texture1D == type )
				dstDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			else if (TextureType::Texture2D == type )
				dstDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.ViewDimension = dstDimension;
		}
	
		switch (dstDimension)
		{
		case D3D11_DSV_DIMENSION_TEXTURE1D:
			dsvDesc.Texture1D.MipSlice = mipstart;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
			dsvDesc.Texture1DArray.MipSlice = mipstart;
			dsvDesc.Texture1DArray.ArraySize = layercount;
			dsvDesc.Texture1DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2D:
			dsvDesc.Texture2D.MipSlice = mipstart;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2DMS:
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
			dsvDesc.Texture2DArray.MipSlice = mipstart;
			dsvDesc.Texture2DArray.ArraySize = layercount;
			dsvDesc.Texture2DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
			dsvDesc.Texture2DMSArray.ArraySize = layercount;
			dsvDesc.Texture2DMSArray.FirstArraySlice = layerstart;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for DepthStenilView!");
			break;
		}
		ID3D11Resource* textureresource=nullptr;
		if (texture1D != nullptr)
			textureresource = texture1D;
		else if (texture2D != nullptr)
			textureresource = texture2D;
		else if (texture3D != nullptr)
			textureresource = texture3D;
		// Create ShaderResouceView
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateDepthStencilView(
			textureresource, &dsvDesc, &pDepthStencilView
		);
		if (result != S_OK)
		{
			QCAT_CORE_ERROR("DepthStencilView Contruction Failed!");
		}
	}

	DX11DepthStencilView::~DX11DepthStencilView()
	{
	}

	void DX11DepthStencilView::Clear(float depth, float stencil)
	{
		auto& gfx = QGfxDeviceDX11::GetInstance()->GetContext();
		if (viewDesc.Format == TextureFormat::DEPTH24STENCIL8)
			gfx->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
		else
			gfx->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH , depth, stencil);
	}
	
	void DX11TextureUtility::CopyTexture2D_(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion)
	{
		Texture_Desc srcTexDesc = srcTex->GetDesc();
		Texture_Desc dstTexDesc = dstTex->GetDesc();
		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);
		uint32_t dstDimension = Utils::GetDimensionFromType(dstTexDesc.Type);
		ID3D11Texture2D* srctexture2D = nullptr;
		ID3D11Texture2D* dsttexture2D = nullptr;

		D3D11_BOX box;
		box.left = boxregion.xoffset;
		box.right = boxregion.width;
		box.top = boxregion.yoffset;
		box.bottom = boxregion.height;
		box.front = 0;
		box.back = 1;

		if (srcDimension != 2 || dstDimension !=2)
			QCAT_CORE_ERROR("One of Texture Dimension isnt 2 : Textire Dimension Error");
		else
		{
			srctexture2D = (ID3D11Texture2D*)srcTex->GetTextureHandle();
			dsttexture2D = (ID3D11Texture2D*)dstTex->GetTextureHandle();

			QGfxDeviceDX11::GetInstance()->GetContext()->CopySubresourceRegion(dsttexture2D, mipLevel, 0, 0, 0, srctexture2D, mipLevel, &box);
		}
	}

	DX11ReadWriteTextureView::DX11ReadWriteTextureView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t startLayer, uint32_t numlayer, Mode mode)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);


		bool pass = true;
		if (srcTexDesc.MipLevels < startMip)
		{
			QCAT_CORE_ERROR("UnorderedView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("UnorderedView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		D3D11_UAV_DIMENSION dstDimension = Utils::GetUAVDimensionFromType(type);

		uavDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		uavDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;
		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layerstart = startLayer;
		uint32_t layercount = numlayer;
		uint32_t mipstart = startMip;
		//if (TextureType::TextureCubeArray == type)
		//{
		//	dstDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		//}

		if (numlayer > 1 || startLayer > 0)
		{
			if (TextureType::Texture1D == type)
				dstDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			else if (TextureType::Texture2D == type)
				dstDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.ViewDimension = dstDimension;
		}

		switch (dstDimension)
		{
		case D3D11_UAV_DIMENSION_TEXTURE1D:
			uavDesc.Texture1D.MipSlice = mipstart;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
			uavDesc.Texture1DArray.MipSlice = mipstart;
			uavDesc.Texture1DArray.ArraySize = layercount;
			uavDesc.Texture1DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE2D:
			uavDesc.Texture2D.MipSlice = mipstart;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
			uavDesc.Texture2DArray.MipSlice = mipstart;
			uavDesc.Texture2DArray.ArraySize = layercount;
			uavDesc.Texture2DArray.FirstArraySlice = layerstart;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE3D:
			uavDesc.Texture3D.MipSlice = startMip;
			uavDesc.Texture3D.FirstWSlice = startLayer;
			uavDesc.Texture3D.WSize = numlayer;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for DepthStenilView!");
			break;
		}
		ID3D11Resource* textureresource = nullptr;
		if (texture1D != nullptr)
			textureresource = texture1D;
		else if (texture2D != nullptr)
			textureresource = texture2D;
		else if (texture3D != nullptr)
			textureresource = texture3D;

		// Create ShaderResouceView
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateUnorderedAccessView(
			textureresource, &uavDesc, &pUnorderedAccessView
		);
		if (result != S_OK)
		{
			QCAT_CORE_ERROR("UnorderedAccessView Contruction Failed!");
		}
	}

	void DX11ReadWriteTextureView::Bind(uint32_t slot) const
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->CSSetUnorderedAccessViews(slot, 1u, pUnorderedAccessView.GetAddressOf(),nullptr);
	}

	void DX11ReadWriteTextureView::UnBind(uint32_t slot) const
	{
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		QGfxDeviceDX11::GetInstance()->GetContext()->CSSetUnorderedAccessViews(slot, 1u, nullUAV, nullptr);
	}

}
